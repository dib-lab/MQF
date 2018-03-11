#include "../gqf.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include "../catch.hpp"
using namespace std;


TEST_CASE( "Merging Cqf") {
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
     qf_insert(&cf2, vals[i], 0, 50,false,false);
   }
   else{
     qf_insert(&cf1, vals[i], 0, 50,false,false);
   }

 }
 qf_merge(&cf1,&cf2,&cf);

 for (uint64_t i = (nvals*2)/3; i <nvals; i++) {
   vals[i]=vals[i]%cf.metadata->range;
   qf_insert(&cf, vals[i], 0, 50,false,false);
   }

 for (uint64_t i = 0; i < nvals; i++) {

   uint64_t count = qf_count_key_value(&cf, vals[i]%cf.metadata->range, 0);
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
