#include "../gqf.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include "../catch.hpp"
using namespace std;

TEST_CASE( "Add tags to items") {
  QF qf;
  for(uint64_t tag_size=2;tag_size<=5;tag_size++){
    uint64_t qbits=16;
    uint64_t num_hash_bits=qbits+8;
    uint64_t maximum_count=(1ULL<<tag_size)-1;
    INFO("Tag size = "<<tag_size<<" max count= "<<maximum_count);
    qf_init(&qf, (1ULL<<qbits), num_hash_bits, tag_size,3, true, "", 2038074761);

    qf_insert(&qf,150,50,false,false);
    CHECK( qf_count_key(&qf,150)==50);
    qf_add_tag(&qf,150,maximum_count,false,false);
    //qf_dump(&qf);
    REQUIRE( qf_get_tag(&qf,150)==maximum_count);
    CHECK(qf_count_key(&qf,150)==50);
    //
    for(uint64_t i=120;i<=149;i++){
      qf_insert(&qf,i,2,false,false);
      CHECK( qf_count_key(&qf,i)==2);
      qf_add_tag(&qf,i,1);
      REQUIRE( qf_get_tag(&qf,i)==1);
      CHECK( qf_count_key(&qf,i)==2);
    }
    //qf_dump(&qf);
    CHECK( qf_get_tag(&qf,150)==maximum_count);
    CHECK( qf_count_key(&qf,150)==50);

    qf_insert(&qf,1500,50,false,false);
    qf_add_tag(&qf,1500,maximum_count,false,false);
    CHECK( qf_get_tag(&qf,1500)==maximum_count);

    qf_insert(&qf,3000,1,false,false);
    qf_add_tag(&qf,3000,maximum_count);
    CHECK( qf_get_tag(&qf,3000)==maximum_count);

    qf_insert(&qf,1500000,1,false,false);
    qf_add_tag(&qf,1500000,maximum_count);
    CHECK( qf_get_tag(&qf,1500000)==maximum_count);



    qf_destroy(&qf,true);
  }
}




TEST_CASE( "Inserting items( repeated 50 times)  and set tags in cqf(90% load factor )") {
  QF qf;
  int tag_size=3;
  uint64_t qbits=7;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<tag_size)-1;
  INFO("Counter size = "<<tag_size<<" max count= "<<maximum_count);
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, tag_size,3, true, "", 2038074761);

  uint64_t nvals = (1ULL<<qbits);
  uint64_t *vals;
  vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));
  for(uint64_t i=0;i<nvals;i++)
  {
    vals[i]=rand();
    vals[i]=(vals[i]<<32)|rand();
    vals[i]=vals[i]%(qf.metadata->range);
  }
  double loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  uint64_t insertedItems=0;
  uint64_t count;
  while(loadFactor<0.9){

    qf_insert(&qf,vals[insertedItems],50,false,false);
    qf_add_tag(&qf,vals[insertedItems],vals[insertedItems]%(maximum_count+1));

    count = qf_get_tag(&qf,vals[insertedItems]);
    CHECK(count == vals[insertedItems]%(maximum_count+1));
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
    for(uint64_t i=0;i<insertedItems;i++)
    {
      count = qf_count_key(&qf, vals[i]);
      REQUIRE(count >= 50);
      count = qf_get_tag(&qf,vals[i]);
      INFO("bug in  "<<i<<" of "<<insertedItems<<" loadFactor "<<loadFactor);
      REQUIRE(count == vals[i]%(maximum_count+1));
    }

  }

  for(uint64_t i=0;i<insertedItems;i++)
  {
    count = qf_count_key(&qf, vals[i]);
    CHECK(count >= 50);
    count = qf_get_tag(&qf,vals[i]);
    CHECK(count == vals[i]%(maximum_count+1));
  }
  QFi qfi;
  qf_iterator(&qf, &qfi, 0);
  do {
    uint64_t key, value, count;
    qfi_get(&qfi, &key, &value, &count);
    count=qf_count_key(&qf, key);
    CHECK(count >= 50);
    count = qf_get_tag(&qf,key);
    CHECK(count == key%(maximum_count+1));
  } while(!qfi_next(&qfi));

  qf_destroy(&qf,true);

}

TEST_CASE( "Removing tags from items(90% load factor )") {
  QF qf;
  int counter_size=2;
  uint64_t qbits=16;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 3,counter_size, true, "", 2038074761);

  uint64_t nvals = (1ULL<<qbits);
  uint64_t *vals;
  vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));
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
  }
  double loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  uint64_t insertedItems=0;
  while(loadFactor<0.9){

    qf_insert(&qf,vals[insertedItems],50,false,false);
    qf_add_tag(&qf,vals[insertedItems],insertedItems%8);
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  }
  uint64_t count;
  for(uint64_t i=0;i<insertedItems;i++)
  {
    if(i%2==0){
      count = qf_count_key(&qf, vals[i]);
      if(count==100){
        printf("coubn ==100\n" );
      }
      qf_remove_tag(&qf,vals[i],false,false);
    }
  }
  for(uint64_t i=0;i<insertedItems;i++)
  {


    if(i%2==1){
    CHECK(qf_get_tag(&qf,vals[i])== i%8);
    }
    else{
      if(qf_get_tag(&qf,vals[i])!=0){
        INFO("ERROR "<<vals[i]<<" tag Not deleted index= "<<i)
        //printf("%lu not delete at index %lu\n", vals[i],i);
      }
      CHECK(qf_get_tag(&qf,vals[i])==0);

    }
  }
  QFi qfi;
  qf_iterator(&qf, &qfi, 0);
  do {
    uint64_t key, value, count;
    qfi_get(&qfi, &key, &value, &count);
    count=qf_count_key(&qf, key);
    CHECK(count >= 50);
  } while(!qfi_next(&qfi));

  qf_destroy(&qf,true);

}

TEST_CASE( "Removing items from cqf with tags(90% load factor )") {
  QF qf;
  int counter_size=2;
  uint64_t qbits=16;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 3,counter_size, true, "", 2038074761);

  uint64_t nvals = (1ULL<<qbits);
  uint64_t *vals;
  vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));
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
  }
  double loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  uint64_t insertedItems=0;
  while(loadFactor<0.9){

    qf_insert(&qf,vals[insertedItems],50,false,false);
    qf_add_tag(&qf,vals[insertedItems],insertedItems%8);
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  }
  uint64_t count;
  for(uint64_t i=0;i<insertedItems;i++)
  {
    if(i%2==0){
      count = qf_count_key(&qf, vals[i]);
      if(count==100){
        printf("coubn ==100\n" );
      }
    qf_remove(&qf,vals[i],50,false,false);
    count = qf_count_key(&qf, vals[i]);
    CHECK(count ==0);
    }
  }
  for(uint64_t i=0;i<insertedItems;i++)
  {
    count = qf_count_key(&qf, vals[i]);

    if(i%2==1){
    CHECK(qf_get_tag(&qf,vals[i])== i%8);
    CHECK(count >= 50);
    }
    else{
      if(count!=0){
        INFO("ERROR "<<vals[i]<<" Not deleted index= "<<i)
        //printf("%lu not delete at index %lu\n", vals[i],i);
      }
      CHECK(count ==0);

    }
  }
  QFi qfi;
  qf_iterator(&qf, &qfi, 0);
  do {
    uint64_t key, value, count;
    qfi_get(&qfi, &key, &value, &count);
    count=qf_count_key(&qf, key);
    CHECK(count >= 50);
  } while(!qfi_next(&qfi));

  qf_destroy(&qf,true);

}
