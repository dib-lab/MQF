#include "../gqf.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include "../catch.hpp"
using namespace std;


TEST_CASE( "Merging mqf") {
  QF cf,cf1,cf2;
 QFi cfi;
 uint64_t qbits = 18;
 uint64_t small_qbits=qbits;
 uint64_t nhashbits = qbits + 8;
 uint64_t small_nhashbits=small_qbits+8;
 uint64_t nslots = (1ULL << qbits);
 uint64_t small_nslots=(1ULL << small_qbits);
 uint64_t nvals = 250*nslots/1000;
 uint64_t *vals;
 uint64_t counter_size=3;
 /* Initialise the CQF */

 INFO("Initialize first cqf size ="<<nslots<<", hashbits="<<nhashbits);
 qf_init(&cf, nslots, nhashbits, 0,counter_size, true, "", 2038074761);
 INFO("Initialize second cqf size ="<<small_nslots<<", hashbits="<<small_nhashbits);

 qf_init(&cf1, small_nslots, small_nhashbits, 0,counter_size, true, "", 2038074761);
 INFO("Initialize third cqf size ="<<small_nslots<<", hashbits="<<small_nhashbits);
 qf_init(&cf2, small_nslots, small_nhashbits, 0,counter_size, true, "", 2038074761);
 /* Generate random values */
 vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));

 for(uint64_t i=0;i<nvals;i++)
 {
   vals[i]=rand();
   vals[i]=(vals[i]<<32)|rand();
 }


 /* Insert vals in the CQF */
 for (uint64_t i = 0; i < (nvals*2)/3; i++) {
   vals[i]=vals[i]%cf1.metadata->range;
   if(i%2==1){
     qf_insert(&cf2, vals[i], 50,false,false);
   }
   else{
     qf_insert(&cf1, vals[i], 50,false,false);
   }

 }
 qf_merge(&cf1,&cf2,&cf);

 for (uint64_t i = (nvals*2)/3; i <nvals; i++) {
   vals[i]=vals[i]%cf.metadata->range;
   qf_insert(&cf, vals[i], 50,false,false);
   }

 for (uint64_t i = 0; i < nvals; i++) {

   uint64_t count = qf_count_key(&cf, vals[i]%cf.metadata->range);
   CHECK(count>=50);
 }

 /* Initialize an iterator */
 qf_iterator(&cf, &cfi, 0);
 do {
   uint64_t key, value, count;
   qfi_get(&cfi, &key, &value, &count);
   CHECK(count>=50);
 } while(!qfi_next(&cfi));

}

TEST_CASE( "Merging Exception") {
  QF cf,cf1,cf2;
 QFi cfi;



 qf_init(&cf, 256, 15, 0,1, true, "", 2038074761);
 qf_init(&cf1, 256, 16, 0,1, true, "", 2038074761);
 qf_init(&cf2, 512, 17, 0,1, true, "", 2038074761);
 /* Generate random values */
 REQUIRE_THROWS(qf_merge(&cf,&cf1,&cf2));

}

TEST_CASE( "resize Exception") {
 QF cf;
 QFi cfi;



 qf_init(&cf, 256, 15, 0,1, true, "", 2038074761);
 /* Generate random values */
 REQUIRE_THROWS(qf_resize(&cf,30));

}

TEST_CASE( "Resize test" ) {
  QF qf;
  int counter_size=2;
  srand (1);
  uint64_t qbits=16;
  uint64_t num_hash_bits=qbits+9;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, true, "", 2038074761);

  uint64_t nvals = (1ULL<<qbits);
  //uint64_t nvals = 3;
  uint64_t *vals;
  uint64_t *nRepetitions;
  vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));
  nRepetitions= (uint64_t*)malloc(nvals*sizeof(nRepetitions[0]));
  uint64_t count;

  for(uint64_t i=0;i<nvals;i++)
  {
    vals[i]=rand();
    vals[i]=(vals[i]<<32)|rand();
    vals[i]=vals[i]%(qf.metadata->range);

    nRepetitions[i]=(rand()%257)+1;
  }
  double loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  uint64_t insertedItems=0;
  while(insertedItems<nvals && loadFactor<0.9){
  //  printf("inserting %lu count = %lu\n",vals[insertedItems],nRepetitions[insertedItems] );
    INFO("Inserting "<< vals[insertedItems] << " Repeated "<<nRepetitions[insertedItems]);
    qf_insert(&qf,vals[insertedItems],nRepetitions[insertedItems],false,false);
    //qf_dump(&qf);
    INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
    count = qf_count_key(&qf, vals[insertedItems]);
    CHECK(count >= nRepetitions[insertedItems]);
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  }

  int old_occupied_size=qf_space(&qf);
  INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems<< " Occupied space "<<old_occupied_size);
  QF* tmp=qf_resize(&qf,18);
  qf=*tmp;
  int new_occupied_size=qf_space(&qf);
  INFO("new occupied size"<<new_occupied_size);
  loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  REQUIRE(new_occupied_size<old_occupied_size);
  while(insertedItems<nvals && loadFactor<0.9){
  //  printf("inserting %lu count = %lu\n",vals[insertedItems],nRepetitions[insertedItems] );
    INFO("Inserting "<< vals[insertedItems] << " Repeated "<<nRepetitions[insertedItems]);
    qf_insert(&qf,vals[insertedItems],nRepetitions[insertedItems]);
    //qf_dump(&qf);
    INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
    count = qf_count_key(&qf, vals[insertedItems]);
    CHECK(count >= nRepetitions[insertedItems]);
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  }

  for(uint64_t i=0;i<insertedItems;i++)
  {
    count = qf_count_key(&qf, vals[i]);
    INFO("value = "<<vals[i]<<" Repeated " <<nRepetitions[i]);
    CHECK(count >= nRepetitions[i]);
  }

  qf_destroy(&qf);

}

TEST_CASE( "comparing mqf") {
  QF cf,cf1,cf2;
 QFi cfi;
 uint64_t qbits = 18;
 uint64_t small_qbits=qbits;
 uint64_t nhashbits = qbits + 8;
 uint64_t small_nhashbits=small_qbits+8;
 uint64_t nslots = (1ULL << qbits);
 uint64_t small_nslots=(1ULL << small_qbits);
 uint64_t nvals = 250*nslots/1000;
 uint64_t *vals;
 uint64_t counter_size=3;
 /* Initialise the CQF */

 INFO("Initialize first cqf size ="<<nslots<<", hashbits="<<nhashbits);
 qf_init(&cf, nslots, nhashbits, 0,counter_size, true, "", 2038074761);
 INFO("Initialize second cqf size ="<<small_nslots<<", hashbits="<<small_nhashbits);

 qf_init(&cf1, small_nslots, small_nhashbits, 0,counter_size, true, "", 2038074761);
 INFO("Initialize third cqf size ="<<small_nslots<<", hashbits="<<small_nhashbits);
 qf_init(&cf2, small_nslots, small_nhashbits, 0,counter_size, true, "", 2038074761);
 /* Generate random values */
 vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));

 for(uint64_t i=0;i<nvals;i++)
 {
   vals[i]=rand();
   vals[i]=(vals[i]<<32)|rand();
 }


 /* Insert vals in the CQF */
 for (uint64_t i = 0; i < (nvals*2)/3; i++) {
   vals[i]=vals[i]%cf1.metadata->range;
   if(i%2==1){
     qf_insert(&cf2, vals[i], 50,false,false);
     qf_insert(&cf, vals[i], 50,false,false);
   }
   else{
     qf_insert(&cf1, vals[i], 50,false,false);
   }

 }
 REQUIRE(qf_equals(&cf,&cf2)==true);
 REQUIRE(qf_equals(&cf1,&cf2)==false);

}

TEST_CASE( "intersect") {
  QF cf,cf1,cf2,cf3;
 QFi cfi;
 uint64_t qbits = 5;
 uint64_t small_qbits=qbits;
 uint64_t nhashbits = qbits + 15;
 uint64_t small_nhashbits=small_qbits+15;
 uint64_t nslots = (1ULL << qbits);
 uint64_t small_nslots=(1ULL << small_qbits);
 uint64_t nvals = 250*nslots/1000;
 uint64_t *vals;
 uint64_t counter_size=3;
 /* Initialise the CQF */

 INFO("Initialize first cqf size ="<<nslots<<", hashbits="<<nhashbits);
 qf_init(&cf, nslots, nhashbits, 0,counter_size, true, "", 2038074761);
 INFO("Initialize second cqf size ="<<small_nslots<<", hashbits="<<small_nhashbits);

 qf_init(&cf1, small_nslots, small_nhashbits, 0,counter_size, true, "", 2038074761);
 INFO("Initialize third cqf size ="<<small_nslots<<", hashbits="<<small_nhashbits);
 qf_init(&cf2, small_nslots, small_nhashbits, 0,counter_size, true, "", 2038074761);

 qf_init(&cf3, nslots, nhashbits, 0,counter_size, true, "", 2038074761);
 /* Generate random values */
 vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));

 for(uint64_t i=0;i<nvals;i++)
 {
   uint64_t newvalue=0;
   while(newvalue==0){
     newvalue=rand();
     newvalue=(newvalue<<32)|rand();
     newvalue=newvalue%(cf1.metadata->range);
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


 /* Insert vals in the CQF */
 for (uint64_t i = 0; i < (nvals*2)/3; i++) {
   vals[i]=vals[i]%cf1.metadata->range;
   if(i%3==0){
     qf_insert(&cf, vals[i], 50,false,false);
   }
   else if(i%3==1){
     qf_insert(&cf1, vals[i], 50,false,false);
   }
   else{
     qf_insert(&cf, vals[i], 50,false,false);
     qf_insert(&cf1, vals[i], 50,false,false);
     qf_insert(&cf2, vals[i], 50,false,false);
   }
 }


 qf_intersect(&cf1,&cf,&cf3);


 // printf("CF\n" );
 // qf_dump(&cf);
 //
 // printf("CF1\n" );
 // qf_dump(&cf1);
 //
 // printf("CF2\n" );
 // qf_dump(&cf2);
 //
 // printf("CF3\n" );
 // qf_dump(&cf3);
 //

 REQUIRE(qf_equals(&cf2,&cf3)==true);

}

TEST_CASE( "subtract") {
  QF cf,cf1,cf2,cf3;
 QFi cfi;
 uint64_t qbits = 18;
 uint64_t small_qbits=qbits;
 uint64_t nhashbits = qbits + 15;
 uint64_t small_nhashbits=small_qbits+15;
 uint64_t nslots = (1ULL << qbits);
 uint64_t small_nslots=(1ULL << small_qbits);
 uint64_t nvals = 250*nslots/1000;
 uint64_t *vals;
 uint64_t counter_size=3;
 /* Initialise the CQF */

 INFO("Initialize first cqf size ="<<nslots<<", hashbits="<<nhashbits);
 qf_init(&cf, nslots, nhashbits, 0,counter_size, true, "", 2038074761);
 INFO("Initialize second cqf size ="<<small_nslots<<", hashbits="<<small_nhashbits);

 qf_init(&cf1, small_nslots, small_nhashbits, 0,counter_size, true, "", 2038074761);
 INFO("Initialize third cqf size ="<<small_nslots<<", hashbits="<<small_nhashbits);
 qf_init(&cf2, small_nslots, small_nhashbits, 0,counter_size, true, "", 2038074761);

 qf_init(&cf3, nslots, nhashbits, 0,counter_size, true, "", 2038074761);
 /* Generate random values */
 vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));

 for(uint64_t i=0;i<nvals;i++)
 {
   uint64_t newvalue=0;
   while(newvalue==0){
     newvalue=rand();
     newvalue=(newvalue<<32)|rand();
     newvalue=newvalue%(cf1.metadata->range);
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


 /* Insert vals in the CQF */
 for (uint64_t i = 0; i < (nvals*2)/3; i++) {
   vals[i]=vals[i]%cf1.metadata->range;
   qf_insert(&cf, vals[i], 100,false,false);
   qf_insert(&cf1, vals[i], 50,false,false);
   qf_insert(&cf2, vals[i], 50,false,false);
 }


 qf_subtract(&cf,&cf1,&cf3);


 // printf("CF\n" );
 // qf_dump(&cf);
 //
 // printf("CF1\n" );
 // qf_dump(&cf1);
 //
 // printf("CF2\n" );
 // qf_dump(&cf2);
 //
 // printf("CF3\n" );
 // qf_dump(&cf3);


 REQUIRE(qf_equals(&cf2,&cf3)==true);

}



TEST_CASE( "Multi Merging mqf") {
 QF cf2,correctCF;

 QF **cf;
 int nqf=10;
 QFi cfi;
 uint64_t qbits = 18;
 uint64_t small_qbits=qbits;
 uint64_t nhashbits = qbits + 8;
 uint64_t small_nhashbits=small_qbits+8;
 uint64_t nslots = (1ULL << qbits);
 uint64_t small_nslots=(1ULL << small_qbits);
 uint64_t nvals = 250*nslots/1000;
 uint64_t *vals;
 uint64_t counter_size=3;
 /* Initialise the CQF */


 cf=new QF*[nqf];
 for(int i=0;i<nqf;i++)
 {
   cf[i]=new QF();
   qf_init(cf[i], small_nslots, small_nhashbits, 0,counter_size, true, "", 2038074761);
 }

 INFO("Initialize first cqf size ="<<nslots<<", hashbits="<<nhashbits);
 qf_init(&cf2, nslots, nhashbits, 0,counter_size, true, "", 2038074761);
 qf_init(&correctCF, nslots, nhashbits, 0,counter_size, true, "", 2038074761);
 INFO("Initialize second cqf size ="<<small_nslots<<", hashbits="<<small_nhashbits);

 /* Generate random values */
 vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));

 for(uint64_t i=0;i<nvals;i++)
 {
   vals[i]=rand();
   vals[i]=(vals[i]<<32)|rand();
 }


 /* Insert vals in the CQF */
 for (uint64_t i = 0; i < nvals; i++) {
   vals[i]=vals[i]%cf[0]->metadata->range;
   qf_insert(cf[i%nqf], vals[i], 50,false,false);
   qf_insert(cf[(i+1)%nqf], vals[i], 50,false,false);
   qf_insert(&correctCF,vals[i],100,false,false);
 }
 qf_multi_merge(cf,nqf,&cf2);

 REQUIRE(qf_equals(&cf2,&correctCF)==true);
 //
 // printf("CF\n" );
 // qf_dump(cf[0]);
 //
 // printf("CF1\n" );
 // qf_dump(cf[1]);
 //
 // printf("CF2\n" );
 // qf_dump(&cf2);




}
