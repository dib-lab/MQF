#ifndef _COUNTING_H
#define _COUNTING_H

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include<cmath>
#include <random>
#include <algorithm>
#include "../gqf.h"
#include "../LayeredMQF.h"
#include "../bufferedMQF.h"
#include "../cqf/gqf.h"
#include "../countmin/countmin.h"
#include "../countmin-khmer/storage.hh"
#include <limits>
#include<cstdint>
#include "../findPrimeNumbers.h"


using namespace std;


class countingStructure{
public:
  string name;
  uint64_t size;
  uint64_t insertionTime;
  uint64_t queryTime;
  double fpr;
  vector<uint64_t> loadingFactors;
  bool failed;
  double fpr5;
  double fpr10;
  double fpr20;
  double fpr30;
  string parameters;
  countingStructure(){
    insertionTime=0;
    queryTime=0;
    fpr=0.0;
    fpr5=0.0;
    fpr10=0.0;
    fpr20=0.0;
    fpr30=0.0;
    size=0;
    name="";
    failed=false;
    parameters="";
  }
  virtual bool insert(uint64_t item,uint64_t count)=0;
  virtual uint64_t query(uint64_t item)=0;
  virtual uint64_t space()=0;
  virtual uint64_t range()=0;
  virtual uint64_t getLoadFactor(){return 0;};

  void print(){
    cout<<name<<"\t"<<size/(1024.0*1024.0)<<"MB\t"<<space()<<"\t"<<fpr<<"\t"<<insertionTime<<"\t"<<queryTime<<endl;
  }
};

class MQF: public countingStructure{
private:
  QF mqf;
public:
  MQF(uint64_t qbits,uint64_t slot_size,uint64_t fixedCounterSize)
  :countingStructure()
  {
    name="MQF";
    qf_init(&mqf, (1ULL<<qbits), qbits+slot_size, 0,fixedCounterSize, 0,true, "", 2038074761);
    size=mqf.metadata->size;
    parameters="Q="+to_string(qbits)+
               ",hashBits="+to_string(qbits+slot_size)+
               ",fixedCounter="+to_string(fixedCounterSize);
  }
  bool insert(uint64_t item,uint64_t count)override{
    return qf_insert(&mqf,item%(mqf.metadata->range),count,false,false);
    ;}
  uint64_t query(uint64_t item)override{
    return qf_count_key(&mqf,item%(mqf.metadata->range));
    }
  uint64_t space()override{
    return qf_space(&mqf);
  }
  uint64_t range()override{
    return mqf.metadata->range;
  }
  uint64_t calculate_slotsUsedInCounting(){
    return slotsUsedInCounting(&mqf);
  }
  QF* get_MQF()
  {
    return &mqf;
  }


};

class LMQF: public countingStructure{
private:
  layeredMQF mqf;
public:
  LMQF(uint64_t singleqbits, uint64_t qbits,uint64_t slot_size,uint64_t fixedCounterSize)
  {
    name="Layered MQF";
    layeredMQF_init(&mqf, (1ULL<<singleqbits),(1ULL<<qbits), qbits+slot_size, 0,fixedCounterSize, true, "", 2038074761);
    size=mqf.firstLayer_singletons->metadata->size+
    mqf.secondLayer->metadata->size;

    parameters="Singletons Q ="+to_string(singleqbits)+
               "Q ="+to_string(qbits)+
               ",hashBits="+to_string(qbits+slot_size)+
               ",fixedCounter="+to_string(fixedCounterSize);

  }
  bool insert(uint64_t item,uint64_t count)override{
    return layeredMQF_insert(&mqf,item%mqf.secondLayer->metadata->range,count,false,false);
    ;}
  uint64_t query(uint64_t item)override{
    return layeredMQF_count_key(&mqf,item%mqf.secondLayer->metadata->range);
    }
  uint64_t space()override{
    return layeredMQF_space(&mqf);
  }
  uint64_t range()override{
    return mqf.secondLayer->metadata->range;
  }
  uint64_t calculate_slotsUsedInCounting(){
    return 0;
    //return slotsUsedInCounting(&mqf);
  }
  layeredMQF* get_MQF()
  {
    return &mqf;
  }

};

class BMQF: public countingStructure{
private:
  bufferedMQF mqf;
public:
  uint64_t bsize;
  BMQF(uint64_t diskqbits, uint64_t qbits,uint64_t slot_size,uint64_t fixedCounterSize)
  :countingStructure()
  {
    name="Buffered MQF";

    //cout<<bsize<<endl;
    bufferedMQF_init(&mqf,(1ULL<<qbits), (1ULL<<diskqbits), diskqbits+slot_size, 0,fixedCounterSize, "tmp.ser");
    bsize=mqf.disk->stxxlBufferSize;
    size=mqf.memoryBuffer->metadata->size
    + mqf.disk->stxxlBufferSize*1024*1024;
  }
  bool insert(uint64_t item,uint64_t count)override{
    return bufferedMQF_insert(&mqf,item%(mqf.disk->metadata->range),count,false,false);
    ;}
  uint64_t query(uint64_t item)override{
    return bufferedMQF_count_key(&mqf,item%(mqf.disk->metadata->range));
    }
  QF* batchQuery(vector<uint64_t> buffer)
  {
    QF *inputBuffer;
    inputBuffer=new QF();
    uint64_t qbits=(uint64_t)log2((uint64_t)buffer.size());
    qf_init(inputBuffer, (1ULL<<(qbits+1)), mqf.memoryBuffer->metadata->key_bits, 0,2,0, true, "", 2038074761);

    for(auto b:buffer)
    {
      qf_insert(inputBuffer,b%(mqf.disk->metadata->range),1);
    }

    bufferedMQF_BatchQuery(&mqf,inputBuffer);

    return inputBuffer;
  }
  uint64_t space()override{
    return bufferedMQF_space(&mqf);
  }
  uint64_t range()override{
    return mqf.disk->metadata->range;
  }
  uint64_t calculate_slotsUsedInCounting(){
    return 0;
    //return slotsUsedInCounting(&mqf);
  }
  bufferedMQF* get_MQF()
  {
    return &mqf;
  }

};

class CQF: public countingStructure{
private:
  cqf::QF ccqf;
public:
  CQF(uint64_t qbits,uint64_t slot_size)
  :countingStructure()
  {
    name="CQF";
    cqf::qf_init(&ccqf, (1ULL<<qbits), qbits+slot_size, 0, true, "", 2038074761);
    size=ccqf.metadata->size;
    parameters="Q="+to_string(qbits)+
               ",hashBits="+to_string(qbits+slot_size);
  }
  bool insert(uint64_t item,uint64_t count)override{

    return cqf::qf_insert(&ccqf,item%(ccqf.metadata->range),0,count,false,false);
    ;}
  uint64_t query(uint64_t item)override{
    return cqf::qf_count_key_value(&ccqf,item%(ccqf.metadata->range),0);
    }
  uint64_t space()override{
    return qf_space(&ccqf);
  }
  uint64_t range()override{
    return ccqf.metadata->range;
  }

};

class countmin: public countingStructure{
private:
  CM_type* counts;
public:
  countmin(uint64_t width,uint64_t height)
  :countingStructure()
  {
    name="CountMin";
    counts=CM_Init(width,height,0);
    size=CM_Size(counts);
    parameters="Width="+to_string(width)+
               ",Height="+to_string(height);
  }
  bool insert(uint64_t item,uint64_t count)override{

    CM_Update(counts,item,count);
    return true;
    //return cqf::qf_insert(&ccqf,item,0,count,false,false);
    ;}
  uint64_t query(uint64_t item)override{
    return (uint64_t)CM_PointEst(counts,item);
    //return cqf::qf_count_key_value(&ccqf,item,0);
    }
  uint64_t space()override{
    return 0;
    //return qf_space(&ccqf);
  }
  uint64_t range()override{
    return  std::numeric_limits<uint64_t>::max();
    //return ccqf.metadata->range;
  }

};


class countminKhmer: public countingStructure{
private:
  ByteStorage* counts;
  uint64_t width;
public:
  countminKhmer(uint64_t width,uint64_t height)
  :countingStructure()
  {
    this->width=width;
    name="CountMinKhmer";
    vector<uint64_t> primes=findNPrimesLargerThanK(height,width);
    counts=new ByteStorage(primes);
    size=0;
    for(auto a: primes){
      size+=a;
    }
    cout<<endl;
    parameters="Width="+to_string(width)+
               ",Height="+to_string(height);
  }
  bool insert(uint64_t item,uint64_t count)override{
    counts->add(item);
    return true;
    //return cqf::qf_insert(&ccqf,item,0,count,false,false);
    ;}
  uint64_t query(uint64_t item)override{
    return counts->get_count(item);
    //return cqf::qf_count_key_value(&ccqf,item,0);
    }
  uint64_t space()override{
    return 0;
    //return qf_space(&ccqf);
  }
  uint64_t range()override{
    return  std::numeric_limits<uint64_t>::max();
    //return ccqf.metadata->range;
  }

};

// class countmin_madoka: public countingStructure{
// private:
//   madoka::Sketch sketch;
// public:
//   countmin_madoka(uint64_t width,uint64_t height)
//   {
//     sketch.create();
//   }
//   bool insert(uint64_t item,uint64_t count)override{
//
//     sketch.inc(&item,sizeof(uint64_t));
//     return true;
//     //return cqf::qf_insert(&ccqf,item,0,count,false,false);
//     ;}
//   uint64_t query(uint64_t item)override{
//     return (uint64_t)sketch.get(&item,sizeof(uint64_t));
//     //return cqf::qf_count_key_value(&ccqf,item,0);
//     }
//   uint64_t space()override{
//     return 0;
//     //return qf_space(&ccqf);
//   }
//   uint64_t range()override{
//     return  std::numeric_limits<uint64_t>::max();
//     //return ccqf.metadata->range;
//   }
//
// };

#endif
