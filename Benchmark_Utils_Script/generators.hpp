#ifndef _generator_H
#define _generator_H
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include<cmath>
#include <random>
#include <set>
#include <algorithm>
#include "seqio.h"
#include "../kmer.h"
#include "../gqf.h"
#include "countingStructure.hpp"

using namespace std;

#define BITMASK(nbits) ((nbits) == 64 ? 0xffffffffffffffff : (1ULL << (nbits)) \
												- 1ULL)

//#define bufferSize 10000000
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

uint64_t ihash(uint64_t key,uint64_t mask)
	{
		key = (~key + (key << 21)) & mask; // key = (key << 21) - key - 1;
		key = key ^ key >> 24;
		key = ((key + (key << 3)) + (key << 8)) & mask; // key * 265
		key = key ^ key >> 14;
		key = ((key + (key << 2)) + (key << 4)) & mask; // key * 21
		key = key ^ key >> 28;
		key = (key + (key << 31)) & mask;
		return key;
	}


class generator{
protected:
  uint64_t num_elements;
  uint64_t range;
  uint64_t generated_elements;

public:
  uint64_t nunique_items;
  string name;
  string parameters;
  set<uint64_t> newItems;
  set<uint64_t> uniq;
  generator(){num_elements=0;generated_elements=0;}
  virtual bool getElement(uint64_t& res){return false;}
  virtual bool hasMore(){cout<<"Here in the base"<<endl;return num_elements==0 || generated_elements<num_elements;}
  uint64_t get_generated_elements(){return generated_elements;}
};

class zipfGenerator: public generator{
private:
  uint64_t curr_element;
  uint64_t curr_count;

  mt19937_64 mt_rand64;
  mt19937 mt_rand;
  zipf_distribution<uint64_t,double> zipf;
  vector<uint64_t> buffer;
  uint64_t bufferTop;

public:

  zipfGenerator(uint64_t num_elements,uint64_t range,double coefficient)
  {
    name="Zipfian distribution";
    parameters="Coefficient = "+to_string(coefficient);
    this->num_elements=num_elements;
    this->range=range;
    curr_count=0;
    nunique_items=0;
    mt_rand64=mt19937_64(time(0));
    mt_rand=mt19937();
    zipf=zipf_distribution<uint64_t,double>(10000,coefficient);
    uint64_t bufferSize=num_elements/20;
    if(bufferSize==0)
      bufferSize=10000000;
    buffer=vector<uint64_t>(bufferSize);
    newItems=set<uint64_t>();
    for(int i=0;newItems.size()<100000;i++)
    {
        newItems.insert((uint64_t)mt_rand64());
    }

    // while(newItems.size()<100000)
    // {
    //   uint64_t tmp;
    //   getElement(tmp);
    //   generated_elements--;
    //   newItems.insert(tmp);
    // }

    bufferTop=0;
    nunique_items=0;
    curr_count=0;
  //  curr_item=0;
    fillBuffer();
    generated_elements=0;
  }
  bool hasMore() override
  {
    if(bufferTop==buffer.size() &&!fillBuffer())
    {
      return false;
    }
    return true;
  }
  bool fillBuffer(){
    if(nunique_items>=num_elements)
    {
      return false;
    }
    for(int i=0;i<buffer.size();i++)
    {
      uint64_t tmp=2;
      if(!generateElement(tmp))
        return false;
      buffer[i]=tmp;
    }
    random_shuffle(buffer.begin(),buffer.end());
    bufferTop=0;
    return true;
  }
  bool generateElement(uint64_t& res)
  {

    if(curr_count==0)
    {
      if(nunique_items==num_elements)
      {
        return false;
      }
      curr_count=zipf(mt_rand);

      do{
        do{
          curr_element=(uint64_t)mt_rand64();
        }while(newItems.find(curr_element)!=newItems.end());
      }while(uniq.find(curr_element)!=uniq.end());
  //    uniq.insert(curr_element);
      nunique_items++;
    }
    res=curr_element;
    curr_count--;

    return true;
  }
  bool getElement(uint64_t& res) override
  {
    // if(!this->hasMore()){
    //   return false;
    // }
    if(bufferTop==buffer.size() &&!fillBuffer())
    {
      return false;
    }
    res=buffer[bufferTop++];
    generated_elements++;
    return true;
  }
};




class kmersGenerator: public generator{
private:


  string currSeq;
  uint32_t pos;
//  kseq_t *seq;
  uint64_t kSize;
  bool end;
  klibpp::KSeq record;
  klibpp::SeqStreamIn* iss;
  vector<klibpp::KSeq> buffRecords;
  uint32_t buffTop;
  uint64_t mask;
	countingStructure* uniqItems;

public:
  kmersGenerator(uint64_t num_elements,string fastaFilePath, uint64_t kSize)
  {
    iss= new klibpp::SeqStreamIn(fastaFilePath.c_str());
    buffRecords=iss->read(10000);
    buffTop=0;
    record=buffRecords[buffTop++];
    currSeq=string(record.seq);
    //currSeq=string(seq->seq.s);
    pos=0;
    name="Real Kmers";
    parameters="File = "+fastaFilePath+
    ",Ksize = "+to_string(kSize);
    end=false;
    this->num_elements=num_elements;
    this->kSize=kSize;
    this->mask=BITMASK(2*kSize);

    uint64_t bufferSize=num_elements/20;
    if(bufferSize==0)
      bufferSize=10000000;

    newItems=set<uint64_t>();
    // uint64_t tmp;
    // generateElement(tmp);
    // newItems.insert(tmp);

    for(int i=0;newItems.size()<10000;i++)
    {
        uint64_t tmp;
        generateElement(tmp);
        newItems.insert(tmp);

    }
		uint64_t estimatedQ=log2((double)num_elements*1.1)+2;

		uniqItems=new MQF(estimatedQ,2*kSize-estimatedQ,2);

		nunique_items=0;
		end=false;

    generated_elements=0;
  }

  bool hasMore() override
  {
    return nunique_items <num_elements && !end;
  }

  bool generateElement(uint64_t& res)
  {
    if(pos>currSeq.size()-kSize)
    {
      if(buffRecords.size()==buffTop)
      {
        buffRecords=iss->read(10000);
        buffTop=0;
        if(iss->eof()){
          end=true;
          return false;
        }
      }
      record=buffRecords[buffTop++];
      pos=0;
      currSeq=string(record.seq);
    //  cout<<currSeq<<endl;
    }
    string kmer=currSeq.substr(pos,kSize);
    // for(int i=0;i<pos;i++)
    // cout<<" ";
    // cout<<kmer<<"\n";
    res=ihash(kmercounting::str_to_int(kmer),mask);
    pos++;
    return true;
  }
  bool getElement(uint64_t& res) override
  {
    if(!hasMore()){
      return false;
    }
    uint64_t curr_element;
    do{
      if(!generateElement(curr_element)){
        return false;
      };
    }while(newItems.find(curr_element)!=newItems.end());

    // if(!generateElement(curr_element)){
    //   return false;
    // };
    res=curr_element;
    generated_elements++;
		if(uniqItems->query(curr_element)==0)
		{
			nunique_items++;
			uniqItems->insert(curr_element,1);
		}
  //  nunique_items++;
    return true;
  }
};


class uniformGenerator: public generator{
private:
  uint64_t curr_element;
  uint64_t curr_count;

  mt19937_64 mt_rand64;
  uint64_t freq;

	vector<uint64_t> buffer;
  uint64_t bufferTop;

public:
  uniformGenerator(uint64_t num_elements,uint64_t range,uint64_t freq)
  {
		name="UniForm Distribution";
    parameters="Freq = "+to_string(freq);

    this->num_elements=num_elements;
    this->range=range;
    curr_count=0;
    this->freq=freq;
    mt_rand64=mt19937_64(time(0));

		uint64_t bufferSize=num_elements/20;
		if(bufferSize==0)
			bufferSize=10000000;
		buffer=vector<uint64_t>(bufferSize);
		newItems=set<uint64_t>();
		for(int i=0;newItems.size()<100000;i++)
		{
				newItems.insert((uint64_t)mt_rand64());
		}
		bufferTop=0;
    nunique_items=0;
    curr_count=0;
  //  curr_item=0;
    fillBuffer();
    generated_elements=0;

  }
	bool hasMore() override
	{
		if(bufferTop==buffer.size() &&!fillBuffer())
		{
			return false;
		}
		return true;
	}

	bool fillBuffer(){
		if(nunique_items>=num_elements)
		{
			return false;
		}
		for(int i=0;i<buffer.size();i++)
		{
			uint64_t tmp=2;
			if(!generateElement(tmp))
				return false;
			buffer[i]=tmp;
		}
		random_shuffle(buffer.begin(),buffer.end());
		bufferTop=0;
		return true;
	}
	bool generateElement(uint64_t& res)
	{

		if(curr_count==0)
		{
			if(nunique_items==num_elements)
			{
				return false;
			}
			curr_count=freq;

			do{
				do{
					curr_element=(uint64_t)mt_rand64();
				}while(newItems.find(curr_element)!=newItems.end());
			}while(uniq.find(curr_element)!=uniq.end());
	//    uniq.insert(curr_element);
			nunique_items++;
		}
		res=curr_element;
		curr_count--;

		return true;
	}


	bool getElement(uint64_t& res) override
  {
    // if(!this->hasMore()){
    //   return false;
    // }
    if(bufferTop==buffer.size() &&!fillBuffer())
    {
      return false;
    }
    res=buffer[bufferTop++];
    generated_elements++;
    return true;
  }
};


#endif /* _utils_H */
