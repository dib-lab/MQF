#include "gqf.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include <fstream>
#include "hashutil.h"
#include "kmer.h"
#include <vector>
#include <chrono>
#include<cmath>
#include <random>
#include <algorithm>
using namespace std;

#define BufferSize 1000000

/** Zipf-like random distribution.
 *
 * "Rejection-inversion to generate variates from monotone discrete
 * distributions", Wolfgang Hörmann and Gerhard Derflinger
 * ACM TOMACS 6.3 (1996): 169-184
 * copied from https://stackoverflow.com/questions/9983239/how-to-generate-zipf-distributed-numbers-efficiently
 *
 */
template<class IntType = unsigned long, class RealType = double>
class zipf_distribution
{
public:
    typedef RealType input_type;
    typedef IntType result_type;

    static_assert(std::numeric_limits<IntType>::is_integer, "");
    static_assert(!std::numeric_limits<RealType>::is_integer, "");

    zipf_distribution(const IntType n=std::numeric_limits<IntType>::max(),
                      const RealType q=1.0)
        : n(n)
        , q(q)
        , H_x1(H(1.5) - 1.0)
        , H_n(H(n + 0.5))
        , dist(H_x1, H_n)
    {}

    IntType operator()(std::mt19937& rng)
    {
        while (true) {
            const RealType u = dist(rng);
            const RealType x = H_inv(u);
            const IntType  k = clamp<IntType>(std::round(x), 1, n);
            if (u >= H(k + 0.5) - h(k)) {
                return k;
            }
        }
    }

private:
    /** Clamp x to [min, max]. */
    template<typename T>
    static constexpr T clamp(const T x, const T min, const T max)
    {
        return std::max(min, std::min(max, x));
    }

    /** exp(x) - 1 / x */
    static double
    expxm1bx(const double x)
    {
        return (std::abs(x) > epsilon)
            ? std::expm1(x) / x
            : (1.0 + x/2.0 * (1.0 + x/3.0 * (1.0 + x/4.0)));
    }

    /** H(x) = log(x) if q == 1, (x^(1-q) - 1)/(1 - q) otherwise.
     * H(x) is an integral of h(x).
     *
     * Note the numerator is one less than in the paper order to work with all
     * positive q.
     */
    const RealType H(const RealType x)
    {
        const RealType log_x = std::log(x);
        return expxm1bx((1.0 - q) * log_x) * log_x;
    }

    /** log(1 + x) / x */
    static RealType
    log1pxbx(const RealType x)
    {
        return (std::abs(x) > epsilon)
            ? std::log1p(x) / x
            : 1.0 - x * ((1/2.0) - x * ((1/3.0) - x * (1/4.0)));
    }

    /** The inverse function of H(x) */
    const RealType H_inv(const RealType x)
    {
        const RealType t = std::max(-1.0, x * (1.0 - q));
        return std::exp(log1pxbx(t) * x);
    }

    /** That hat function h(x) = 1 / (x ^ q) */
    const RealType h(const RealType x)
    {
        return std::exp(-q * std::log(x));
    }

    static constexpr RealType epsilon = 1e-8;

    IntType                                  n;     ///< Number of elements
    RealType                                 q;     ///< Exponent
    RealType                                 H_x1;  ///< H(x_1)
    RealType                                 H_n;   ///< H(n)
    std::uniform_real_distribution<RealType> dist;  ///< [H(x_1), H(n)]
};

int main(int argc, char const *argv[]) {

  //ifstream dataset(argv[1]);
  string distribution=argv[1];
  uint64_t NumElements=atoi(argv[2]);
  double parameter=atof(argv[3]);


  uint64_t hash;
  zipf_distribution<uint64_t,double> zipi(10000,1.5);
  std::mt19937_64 mt_rand_64(time(0));
  mt19937 mt_rand(time(0));
  uint64_t curr_count=0;
  uint64_t curr_item=0;
  uint64_t old_capacity=capacity;
  uint64_t insertiontime=0;
  uint64_t querytime=0;
  while(capacity<95)
  {
    for(int j=0;j<BufferSize;j++){
      if(curr_count==0){
        curr_count=zipi(mt_rand);
        curr_item=(uint64_t)mt_rand_64()%(qf.metadata->range);
      }
      input[j]=curr_item;
      curr_count--;
    }
  //  sort(input.begin(),input.end());
    now = std::chrono::high_resolution_clock::now();
    for(int j=0;j<BufferSize;j++)
      qf_insert(&qf,input[j],1,false,false);
    auto prev=now;
    now = std::chrono::high_resolution_clock::now();
    auto microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
    microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
    insertiontime+=microseconds;
    countedKmers+=BufferSize;

    capacity=qf_space(&qf);

    now = std::chrono::high_resolution_clock::now();
    for(int j=0;j<BufferSize;j++)
    {
      qf_count_key(&qf,input[j]);
    }
    prev=now;
    now = std::chrono::high_resolution_clock::now();
    microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
    querytime+=microseconds;
    if(old_capacity!=capacity){
      double millionInsertionSecond=(double)countedKmers/(double)insertiontime;
      cout<<capacity<<"\t"<<millionInsertionSecond<<endl;
      old_capacity=capacity;

      double millionQuerysSecond=(double)countedKmers/(double)querytime;
      result.push_back(make_pair(capacity,(double)millionQuerysSecond));
    }




  }
  cout<<"Capacity\t1M Query per second"<<endl;
  for(int i=0;i<result.size();i++)
  {
    cout<<result[i].first<<"\t"<<result[i].second<<endl;
  }
//  qf_destroy(&qf);
  return 0;
}
