#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include<cmath>
#include <random>
#include <algorithm>
using namespace std;


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

class generator{
protected:
  uint64_t num_elements;
  uint64_t range;
  uint64_t generated_elements;
public:
  generator(){num_elements=0;generated_elements=0;}
  virtual bool getElement(uint64_t& res){return false;}
  bool hasMore(){return num_elements==0 || generated_elements<num_elements;}
  uint64_t get_generated_elements(){return generated_elements;}
};

class zipfGenerator: public generator{
private:
  uint64_t curr_element;
  uint64_t curr_count;

  mt19937_64 mt_rand64;
  mt19937 mt_rand;
  zipf_distribution<uint64_t,double> zipf;
public:
  zipfGenerator(uint64_t num_elements,uint64_t range,double coefficient)
  {
    this->num_elements=num_elements;
    this->range=range;
    curr_count=0;
    mt_rand64=mt19937_64(time(0));
    mt_rand=mt19937();
    zipf=zipf_distribution<uint64_t,double>(10000,coefficient);
  }

  bool getElement(uint64_t& res) override
  {
    if(!this->hasMore()){
      return false;
    }
    if(curr_count==0)
    {
      curr_count=zipf(mt_rand);
      curr_element=(uint64_t)mt_rand64()%(range);
    }
    res=curr_element;
    curr_count--;
    generated_elements++;
    return true;
  }
};

class uniformGenerator: public generator{
private:
  uint64_t curr_element;
  uint64_t curr_count;

  mt19937_64 mt_rand64;
  uint64_t freq;
public:
  uniformGenerator(uint64_t num_elements,uint64_t range,uint64_t freq)
  {
    this->num_elements=num_elements;
    this->range=range;
    curr_count=0;
    this->freq=freq;
    mt_rand64=mt19937_64(time(0));
  }

  bool getElement(uint64_t& res) override
  {
    if(!this->hasMore()){
      return false;
    }
    if(curr_count==0)
    {
      curr_count=freq;
      curr_element=(uint64_t)mt_rand64()%(range);
    }
    res=curr_element;
    curr_count--;
    generated_elements++;
    return true;
  }
};
