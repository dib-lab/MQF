#include "gqf.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include <unordered_map>
#include "catch.hpp"
using namespace std;

TEST_CASE( "Add labels to items") {
  QF qf;
  for(uint64_t label_size=2;label_size<=5;label_size++){
    uint64_t qbits=16;
    uint64_t num_hash_bits=qbits+8;
    uint64_t maximum_count=(1ULL<<label_size)-1;
    INFO("Label size = "<<label_size<<" max count= "<<maximum_count);
    qf_init(&qf, (1ULL<<qbits), num_hash_bits, label_size,3,0, true, "", 2038074761);

    qf_insert(&qf,150,50,false,false);
    CHECK( qf_count_key(&qf,150)==50);
    qf_add_label(&qf,150,maximum_count,false,false);
    //qf_dump(&qf);
    REQUIRE( qf_get_label(&qf,150)==maximum_count);
    CHECK(qf_count_key(&qf,150)==50);
    //
    for(uint64_t i=120;i<=149;i++){
      qf_insert(&qf,i,2,false,false);
      CHECK( qf_count_key(&qf,i)==2);
      qf_add_label(&qf,i,1);
      REQUIRE( qf_get_label(&qf,i)==1);
      CHECK( qf_count_key(&qf,i)==2);
    }
    //qf_dump(&qf);
    CHECK( qf_get_label(&qf,150)==maximum_count);
    CHECK( qf_count_key(&qf,150)==50);

    qf_insert(&qf,1500,50,false,false);
    qf_add_label(&qf,1500,maximum_count,false,false);
    CHECK( qf_get_label(&qf,1500)==maximum_count);

    qf_insert(&qf,3000,1,false,false);
    qf_add_label(&qf,3000,maximum_count);
    CHECK( qf_get_label(&qf,3000)==maximum_count);

    qf_insert(&qf,1500000,1,false,false);
    qf_add_label(&qf,1500000,maximum_count);
    CHECK( qf_get_label(&qf,1500000)==maximum_count);



    qf_destroy(&qf);
  }
}


TEST_CASE( "Add labels to Blocks") {
  QF qf;
  for(uint64_t label_size=1;label_size<=128;label_size*=2){
    uint64_t qbits=16;
    uint64_t num_hash_bits=qbits+8;
    uint64_t maximum_count=(1ULL<<label_size)-1;
    INFO("Label size = "<<label_size<<" max count= "<<maximum_count);
    qf_init(&qf, (1ULL<<qbits), num_hash_bits, 0,3,label_size, true, "", 2038074761);

    qf_insert(&qf,150,50,false,false);
    CHECK( qf_count_key(&qf,150)==50);
    char* ptr=qf_getBlockLabel_pointer_byBlock(&qf,0);
    for(int i=0;i<label_size;i++)
      ptr[i]=(char)((int)'a'+i);
    //qf_add_label(&qf,150,maximum_count,false,false);
    //qf_dump(&qf);
    //REQUIRE( qf_get_label(&qf,150)==maximum_count);
    char* ptr2=qf_getBlockLabel_pointer_byBlock(&qf,0);
    for(int i=0;i<label_size;i++)
      REQUIRE(ptr2[i]==(char)((int)'a'+i));
    CHECK(qf_count_key(&qf,150)==50);
    //



    qf_destroy(&qf);
  }
}



TEST_CASE( "Inserting items( repeated 50 times)  and set block labels in cqf(90% load factor )") {
  QF qf;
  int label_size=8;
  uint64_t qbits=15;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<label_size)-1;
  INFO("Counter size = "<<label_size<<" max count= "<<maximum_count);
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 0,3,label_size, true, "", 2038074761);

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
  //  qf_add_label(&qf,vals[insertedItems],vals[insertedItems]%(maximum_count+1));

  //  count = qf_get_label(&qf,vals[insertedItems]);
  //  CHECK(count == vals[insertedItems]%(maximum_count+1));
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;

  }
  for(int i=0;i<qf.metadata->nblocks;i++)
  {
    char* blockLabel=qf_getBlockLabel_pointer_byBlock(&qf,i);
    for(int j=0;j<label_size;j++){
      char newChar=(char)((int)'a'+i+j);
      if(newChar>'z')
        newChar='a';
    //  cout<<newChar;
      blockLabel[j]=newChar;
    }
  //  cout<<endl;
  }
//  cout<<endl;
  for(uint64_t i=0;i<insertedItems;i++)
  {
    count = qf_count_key(&qf, vals[i]);
    CHECK(count >= 50);
  }
  QFi qfi;
  qf_iterator(&qf, &qfi, 0);
  do {
    uint64_t key, value, count;
    qfi_get(&qfi, &key, &value, &count);
    count=qf_count_key(&qf, key);
    // CHECK(count >= 50);
    // count = qf_get_label(&qf,key);
    // CHECK(count == key%(maximum_count+1));
  } while(!qfi_next(&qfi));

  for(int i=0;i<qf.metadata->nblocks;i++)
  {
    char* blockLabel=qf_getBlockLabel_pointer_byBlock(&qf,i);
    for(int j=0;j<label_size;j++){
      char newChar=(char)((int)'a'+i+j);
      if(newChar>'z')
        newChar='a';
      //cout<<newChar;
      REQUIRE(blockLabel[j]==newChar);
    }
    //cout<<endl;
  }

  qf_destroy(&qf);

}
TEST_CASE( "Inserting items( repeated 50 times)  and compute kmers order(90% load factor )") {
  QF qf;
  int label_size=32;
  uint64_t qbits=15;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<label_size)-1;
  INFO("Counter size = "<<label_size<<" max count= "<<maximum_count);
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 0,3,label_size, true, "", 2038074761);

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
  //  qf_add_label(&qf,vals[insertedItems],vals[insertedItems]%(maximum_count+1));

  //  count = qf_get_label(&qf,vals[insertedItems]);
  //  CHECK(count == vals[insertedItems]%(maximum_count+1));
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;

  }

//  cout<<endl;
  for(uint64_t i=0;i<insertedItems;i++)
  {
    count = qf_count_key(&qf, vals[i]);
    CHECK(count >= 50);
  }
  qf_ComputeItemsOrder(&qf);
  QFi qfi;
  uint64_t expectedOrder=0;
  qf_iterator(&qf, &qfi, 0);
  do {
    uint64_t key, value, count;
    qfi_get(&qfi, &key, &value, &count);
    count=qf_count_key(&qf, key);
    uint64_t order=itemOrder(&qf,key);
    CHECK(count >= 50);
    REQUIRE(order==expectedOrder);
    expectedOrder++;
    // count = qf_get_label(&qf,key);
    // CHECK(count == key%(maximum_count+1));
  } while(!qfi_next(&qfi));


  qf_destroy(&qf);

}

TEST_CASE( "Inserting items( repeated 50 times)  and attach  label block to items in cqf(90% load factor )") {
  QF qf;
  int label_size=8;
  uint64_t qbits=15;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<label_size)-1;
  INFO("Counter size = "<<label_size<<" max count= "<<maximum_count);
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 0,3,label_size, true, "", 2038074761);

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
  unordered_map<uint64_t,uint64_t> labels_Map;
  uint64_t currLabel=0;
  while(loadFactor<0.9){
    qf_insert(&qf,vals[insertedItems],50,false,false);
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  }

//  cout<<endl;
  QFi qfi;
  qf_iterator(&qf, &qfi, 0);
  do {
    uint64_t key, value, count;
    qfi_get(&qfi, &key, &value, &count);

    char* label;
    bool flag=qf_getBlockLabel_pointer_byItem(&qf,key,label);
    REQUIRE(flag==true);
    uint64_t block_index = qfi.current/64;
    auto it=labels_Map.find(block_index);
    if(it==labels_Map.end())
    {
      labels_Map[block_index]=currLabel++;
    }
    *((uint64_t*)label)=labels_Map[block_index];

  } while(!qfi_next(&qfi));
  qf_iterator(&qf, &qfi, 0);
  do {
    uint64_t key, value, count;
    qfi_get(&qfi, &key, &value, &count);
    count=qf_count_key(&qf, key);
    CHECK(count >= 50);
    char* label;
    bool flag=qf_getBlockLabel_pointer_byItem(&qf,key,label);
    REQUIRE(flag==true);
    uint64_t block_index = qfi.current/64;
    CHECK(labels_Map[block_index]==*((uint64_t*)label));


    // count = qf_get_label(&qf,key);
    // CHECK(count == key%(maximum_count+1));
  } while(!qfi_next(&qfi));



  qf_destroy(&qf);

}


TEST_CASE( "Inserting items( repeated 50 times)  and set labels in cqf(90% load factor )") {
  QF qf;
  int label_size=3;
  uint64_t qbits=7;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<label_size)-1;
  INFO("Counter size = "<<label_size<<" max count= "<<maximum_count);
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, label_size,3,0, true, "", 2038074761);

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
    qf_add_label(&qf,vals[insertedItems],vals[insertedItems]%(maximum_count+1));

    count = qf_get_label(&qf,vals[insertedItems]);
    CHECK(count == vals[insertedItems]%(maximum_count+1));
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
    for(uint64_t i=0;i<insertedItems;i++)
    {
      count = qf_count_key(&qf, vals[i]);
      REQUIRE(count >= 50);
      count = qf_get_label(&qf,vals[i]);
      INFO("bug in  "<<i<<" of "<<insertedItems<<" loadFactor "<<loadFactor);
      REQUIRE(count == vals[i]%(maximum_count+1));
    }

  }

  for(uint64_t i=0;i<insertedItems;i++)
  {
    count = qf_count_key(&qf, vals[i]);
    CHECK(count >= 50);
    count = qf_get_label(&qf,vals[i]);
    CHECK(count == vals[i]%(maximum_count+1));
  }
  QFi qfi;
  qf_iterator(&qf, &qfi, 0);
  do {
    uint64_t key, value, count;
    qfi_get(&qfi, &key, &value, &count);
    count=qf_count_key(&qf, key);
    CHECK(count >= 50);
    count = qf_get_label(&qf,key);
    CHECK(count == key%(maximum_count+1));
  } while(!qfi_next(&qfi));

  qf_destroy(&qf);

}

TEST_CASE( "Removing labels from items(90% load factor )") {
  QF qf;
  int counter_size=2;
  uint64_t qbits=16;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 3,counter_size,0, true, "", 2038074761);

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
    qf_add_label(&qf,vals[insertedItems],insertedItems%8);
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
      qf_remove_label(&qf,vals[i],false,false);
    }
  }
  for(uint64_t i=0;i<insertedItems;i++)
  {


    if(i%2==1){
    CHECK(qf_get_label(&qf,vals[i])== i%8);
    }
    else{
      if(qf_get_label(&qf,vals[i])!=0){
        INFO("ERROR "<<vals[i]<<" label Not deleted index= "<<i)
        //printf("%lu not delete at index %lu\n", vals[i],i);
      }
      CHECK(qf_get_label(&qf,vals[i])==0);

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

  qf_destroy(&qf);

}

TEST_CASE( "Removing items from cqf with labels(90% load factor )") {
  QF qf;
  int counter_size=2;
  uint64_t qbits=16;
  uint64_t num_hash_bits=qbits+11;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 3,counter_size,0, true, "", 2038074761);

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
    qf_add_label(&qf,vals[insertedItems],insertedItems%8);
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
    CHECK(qf_get_label(&qf,vals[i])== i%8);
    CHECK(count >= 50);
    }
    else{
      if(count!=0){
        INFO("ERROR "<<vals[i]<<" Not deleted index= "<<i)
        printf("%lu not delete at index %lu\n", vals[i],i);
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

  qf_destroy(&qf);

}
