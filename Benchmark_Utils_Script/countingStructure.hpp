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
#include "../cqf/gqf.h"
#include "../countmin/countmin.h"
#include <limits>
#include<cstdint>
#include "madoka/lib/madoka.h"
// #include "oxli/oxli.hh"
// #include "oxli/hashtable.hh"




class countingStructure{
public:
  countingStructure(){}
  virtual bool insert(uint64_t item,uint64_t count)=0;
  virtual uint64_t query(uint64_t item)=0;
  virtual uint64_t space()=0;
  virtual uint64_t range()=0;
};

class MQF: public countingStructure{
private:
  QF mqf;
public:
  MQF(uint64_t qbits,uint64_t slot_size,uint64_t fixedCounterSize)
  {
    qf_init(&mqf, (1ULL<<qbits), qbits+slot_size, 0,fixedCounterSize, true, "", 2038074761);
  }
  bool insert(uint64_t item,uint64_t count)override{
    return qf_insert(&mqf,item,count,false,false);
    ;}
  uint64_t query(uint64_t item)override{
    return qf_count_key(&mqf,item);
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

};

class CQF: public countingStructure{
private:
  cqf::QF ccqf;
public:
  CQF(uint64_t qbits,uint64_t slot_size)
  {
    cqf::qf_init(&ccqf, (1ULL<<qbits), qbits+slot_size, 0, true, "", 2038074761);
  }
  bool insert(uint64_t item,uint64_t count)override{
    return cqf::qf_insert(&ccqf,item,0,count,false,false);
    ;}
  uint64_t query(uint64_t item)override{
    return cqf::qf_count_key_value(&ccqf,item,0);
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
  {
    counts=CM_Init(width,height,0);
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
class countmin_madoka: public countingStructure{
private:
  madoka::Sketch sketch;
public:
  countmin_madoka(uint64_t width,uint64_t height)
  {
    sketch.create();
  }
  bool insert(uint64_t item,uint64_t count)override{

    sketch.inc(&item,sizeof(uint64_t));
    return true;
    //return cqf::qf_insert(&ccqf,item,0,count,false,false);
    ;}
  uint64_t query(uint64_t item)override{
    return (uint64_t)sketch.get(&item,sizeof(uint64_t));
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
