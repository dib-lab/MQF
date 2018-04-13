#include "../gqf.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include "../catch.hpp"
using namespace std;

TEST_CASE( "approx count test") {
  QF qf;
  int counter_size=3;
  uint64_t qbits=8;
  uint64_t num_hash_bits=qbits+10;
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
    qf_insert(&qf,vals[insertedItems],nRepetitions[insertedItems],false,false);
    qf_add_tag(&qf,vals[insertedItems],insertedItems%8);
    count = qf_count_key(&qf, vals[insertedItems]);
    //CHECK(count >= nRepetitions[insertedItems]);
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  }
  INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
  INFO("nslots ="<<qf.metadata->nslots);
  qf_serialize(&qf,"tmp.ser");
  qf_destroy(&qf);
  cout<<"Create Finsihed"<<endl;
  uint64_t index_hashbits=qbits+5;
  uint64_t slot_shift=num_hash_bits-index_hashbits;
  uint64_t *min_counts,*max_counts;
  min_counts = (uint64_t*)malloc(nvals*sizeof(min_counts[0]));
  max_counts = (uint64_t*)malloc(nvals*sizeof(max_counts[0]));
  uint64_t tmp=(1ULL << slot_shift)-1;
  for(int i=0;i<nvals;i++){
    min_counts[i]=nRepetitions[i]>>slot_shift;
    min_counts[i]=nRepetitions[i]<<slot_shift;
    max_counts[i]=min_counts[i]+tmp;
  }
  uint64_t min_count,max_count;
  for(int i=0;i<nvals;i++)
  {
      min_count=min_counts[i];
      max_count=max_counts[i];
      for(int j=0;j<nvals;j++){
        if(vals[i]>>slot_shift == vals[j]>>slot_shift){
          min_count=min(min_count,min_counts[j]);
          max_count=max(max_count,max_counts[j]);
        }
      }
      min_counts[i]=min_count;
      max_counts[i]=max_count;
  }
  cout<<"Simulate finished"<<endl;
  qf_index* index;
  index=(qf_index*)calloc(sizeof(qf_index),1);
  index->main_qf=(QF*)calloc(sizeof(QF),1);
  index->index_qf=(QF*)calloc(sizeof(QF),1);
  qf_index_init(index,"tmp.ser",5,3);
  printf("main bits per slot %llu\n",index->main_qf->metadata->bits_per_slot );

  cout<<"index initialize finished"<<endl;
  for(int i=0;i<nvals;i++)
  {
      approx_count_range(index,vals[i],&min_count,&max_count);
      CHECK(min_counts[i]==min_count);
      CHECK(max_counts[i]==max_count);
      cout<<min_count<<endl;
  }





}
