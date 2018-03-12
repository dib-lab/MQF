#include "../gqf.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include "../catch.hpp"
using namespace std;

TEST_CASE( "Writing and Reading to/from Disk") {
  QF qf;
  int counter_size=2;
  uint64_t qbits=16;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 3,counter_size, true, "", 2038074761);

  uint64_t nvals = (1ULL<<qbits);
  //uint64_t nvals = 3;
  uint64_t *vals;
  uint64_t *nRepetitions;
  vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));
  nRepetitions= (uint64_t*)malloc(nvals*sizeof(nRepetitions[0]));
  uint64_t count;

  for(uint64_t i=0;i<nvals;i++)
  {
    uint64_t newvalue=0;
    while(newvalue==0){
      newvalue=rand();
      newvalue=(newvalue<<32)|rand();
      newvalue=newvalue%(qf.metadata->range);
      for(uint64_t j=0;j<i;j++)
      {
        if(vals[j]==newvalue)
        {
          newvalue=0;
          break;
        }
      }
    }
    vals[i]=newvalue;

    nRepetitions[i]=(rand()%257)+1;
  }
  double loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  uint64_t insertedItems=0;
  while(insertedItems<nvals && loadFactor<0.9){
    qf_insert(&qf,vals[insertedItems],0,nRepetitions[insertedItems],false,false);
    qf_add_tag(&qf,vals[insertedItems],insertedItems%8);
    count = qf_count_key_value(&qf, vals[insertedItems], 0);
    CHECK(count >= nRepetitions[insertedItems]);
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  }
  INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
  INFO("nslots ="<<qf.metadata->nslots);
  qf_serialize(&qf,"tmp.ser");
  qf_destroy(&qf,true);

  SECTION("Reading using qf_read(mmap)"){
    QF qf2;
    qf_read(&qf2,"tmp.ser");
    INFO("nslots ="<<qf2.metadata->nslots);
    for(uint64_t i=0;i<insertedItems;i++)
    {
      count = qf_count_key_value(&qf2, vals[i], 0);
      INFO("value = "<<vals[i]<<" Repeated " <<nRepetitions[i]);
      CHECK(count >= nRepetitions[i]);
      CHECK(qf_get_tag(&qf2,vals[i])== i%8);
    }

    qf_destroy(&qf2,false);
  }

  SECTION("Reading using deserialize "){
    qf_deserialize(&qf,"tmp.ser");

    for(uint64_t i=0;i<insertedItems;i++)
    {
      count = qf_count_key_value(&qf, vals[i], 0);
      INFO("value = "<<vals[i]<<" Repeated " <<nRepetitions[i]);
      CHECK(count >= nRepetitions[i]);
      CHECK(qf_get_tag(&qf,vals[i])== i%8);
    }

    qf_destroy(&qf,true);
  }



}

TEST_CASE( "MMap test") {
  QF qf;
  int counter_size=2;
  uint64_t qbits=16;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 3,counter_size, false, "tmp.ser", 2038074761);

  uint64_t nvals = (1ULL<<qbits);
  //uint64_t nvals = 3;
  uint64_t *vals;
  uint64_t *nRepetitions;
  vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));
  nRepetitions= (uint64_t*)malloc(nvals*sizeof(nRepetitions[0]));
  uint64_t count;

  for(uint64_t i=0;i<nvals;i++)
  {
    uint64_t newvalue=0;
    while(newvalue==0){
      newvalue=rand();
      newvalue=(newvalue<<32)|rand();
      newvalue=newvalue%(qf.metadata->range);
      for(uint64_t j=0;j<i;j++)
      {
        if(vals[j]==newvalue)
        {
          newvalue=0;
          break;
        }
      }
    }
    vals[i]=newvalue;

    nRepetitions[i]=(rand()%257)+1;
  }
  double loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  uint64_t insertedItems=0;
  while(insertedItems<nvals && loadFactor<0.9){
    qf_insert(&qf,vals[insertedItems],0,nRepetitions[insertedItems],false,false);
    qf_add_tag(&qf,vals[insertedItems],insertedItems%8);
    count = qf_count_key_value(&qf, vals[insertedItems], 0);
    CHECK(count >= nRepetitions[insertedItems]);
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  }
  INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems<<" "<<vals[0]);

  for(uint64_t i=0;i<insertedItems;i++)
  {
    INFO("Check = "<<vals[i]);
    count = qf_count_key_value(&qf, vals[i], 0);
    INFO("value = "<<vals[i]<<" Repeated " <<nRepetitions[i]);
    CHECK(count >= nRepetitions[i]);
    CHECK(qf_get_tag(&qf,vals[i])== i%8);
  }

  qf_destroy(&qf,false);

}
