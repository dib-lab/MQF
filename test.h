#define CATCH_CONFIG_MAIN

#include "catch.hpp"


TEST_CASE( "get/set fixed counters" ) {
  QF qf;
  for(int counter_size=1;counter_size<=5;counter_size++){
    uint64_t qbits=15;
    uint64_t num_hash_bits=qbits+8;
    uint64_t maximum_count=(1ULL<<counter_size)-1;
    INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
    qf_init(&qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, true, "", 2038074761);
    uint64_t c;
    // test many set and get
    uint64_t last=0;
    for(int i=1;i<=maximum_count;i++){
      REQUIRE( get_fixed_counter(&qf,99) == 0 );
      CHECK( get_fixed_counter(&qf,100) == last );
      REQUIRE( get_fixed_counter(&qf,101) == 0 );
      set_fixed_counter(&qf,100,i);
      REQUIRE( get_fixed_counter(&qf,99) == 0 );
      CHECK( get_fixed_counter(&qf,100) == i );
      REQUIRE( get_fixed_counter(&qf,101) == 0 );
      last=i;
    }

    //test on block boundaries

    REQUIRE( get_fixed_counter(&qf,63) == 0 );
    REQUIRE( get_fixed_counter(&qf,64) == 0 );
    REQUIRE( get_fixed_counter(&qf,65) == 0 );

    c=1;
    set_fixed_counter(&qf,63,c);
    c=(c+1)%maximum_count;
    set_fixed_counter(&qf,64,c);
    c=(c+1)%maximum_count;
    set_fixed_counter(&qf,65,c);

    c=1;
    REQUIRE( get_fixed_counter(&qf,63) == c );
    c=(c+1)%maximum_count;
    REQUIRE( get_fixed_counter(&qf,64) == c );
    c=(c+1)%maximum_count;
    REQUIRE( get_fixed_counter(&qf,65) == c );

    // test special slots
    c=1;
    uint64_t special_slots[]={
      0,
      (1ULL<qbits),
      qf.metadata->xnslots-1
    };
    for(int i=0;i<3;i++){
      INFO("Testing Special Slot "<<special_slots[i]);
      REQUIRE( get_fixed_counter(&qf,special_slots[i]) == 0 );
      set_fixed_counter(&qf,special_slots[i],c);
      REQUIRE( get_fixed_counter(&qf,special_slots[i]) == c );
    }

    qf_destroy(&qf,true);
  }
}

TEST_CASE( "shift fixed counters" ) {
  QF qf;
  for(int counter_size=1;counter_size<=5;counter_size++){
    uint64_t qbits=15;
    uint64_t num_hash_bits=qbits+8;
    uint64_t maximum_count=(1ULL<<counter_size)-1;
    INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
    qf_init(&qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, true, "", 2038074761);
    uint64_t c;
    // test  shift one item
    uint64_t last=0;
    set_fixed_counter(&qf,100,1);
    CHECK( get_fixed_counter(&qf,100) == 1 );
    shift_fixed_counters(&qf,100,100,1);
    CHECK( get_fixed_counter(&qf,100) == 0 );
    CHECK( get_fixed_counter(&qf,101) == 1 );

    //test shift many items
    set_fixed_counter(&qf,100,maximum_count);
    set_fixed_counter(&qf,99,maximum_count);
    shift_fixed_counters(&qf,99,101,5);

    CHECK( get_fixed_counter(&qf,99) == 0 );
    CHECK( get_fixed_counter(&qf,100) == 0 );
    CHECK( get_fixed_counter(&qf,101) == 0 );

    CHECK( get_fixed_counter(&qf,104) == maximum_count );
    CHECK( get_fixed_counter(&qf,105) == maximum_count );
    CHECK( get_fixed_counter(&qf,106) == 1 );

    // test shift on block boundary
    set_fixed_counter(&qf,63,maximum_count);
    set_fixed_counter(&qf,64,maximum_count);
    set_fixed_counter(&qf,65,maximum_count);

    shift_fixed_counters(&qf,63,65,5);

    CHECK( get_fixed_counter(&qf,63) == 0 );
    CHECK( get_fixed_counter(&qf,64) == 0 );
    CHECK( get_fixed_counter(&qf,65) == 0 );

    CHECK( get_fixed_counter(&qf,68) == maximum_count );
    CHECK( get_fixed_counter(&qf,69) == maximum_count );
    CHECK( get_fixed_counter(&qf,70) == maximum_count );

    qf_destroy(&qf,true);
  }
}


TEST_CASE( "Adding fixed counters to items" ) {
  QF qf;
  for(int counter_size=1;counter_size<=5;counter_size++){
    uint64_t qbits=15;
    uint64_t num_hash_bits=qbits+8;
    uint64_t maximum_count=(1ULL<<counter_size)-1;
    INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
    qf_init(&qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, true, "", 2038074761);

    qf_insert(&qf,150,0,1,false,false);
    CHECK( qf_count_key_value(&qf,150,0)==1);

    qf_set_fixed_counter(&qf,150,maximum_count);
    //  CHECK( qf_get_fixed_counter(&qf,150)==maximum_count);
    CHECK( qf_count_key_value(&qf,150,0)==1);

    qf_insert(&qf,1500,0,1,false,false);
    qf_set_fixed_counter(&qf,1500,maximum_count);
    CHECK( qf_get_fixed_counter(&qf,1500)==maximum_count);

    qf_insert(&qf,3000,0,1,false,false);
    qf_set_fixed_counter(&qf,3000,maximum_count);
    CHECK( qf_get_fixed_counter(&qf,3000)==maximum_count);

    qf_insert(&qf,1500000,0,1,false,false);
    qf_set_fixed_counter(&qf,1500000,maximum_count);
    CHECK( qf_get_fixed_counter(&qf,1500000)==maximum_count);



    qf_destroy(&qf,true);
  }
}

TEST_CASE( "Inserting items in cqf(90% load factor )" ) {
  QF qf;
  int counter_size=2;
  uint64_t qbits=15;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, true, "", 2038074761);

  uint64_t nvals = (1ULL<<qbits);
  uint64_t *vals;
  vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));
  for(int i=0;i<nvals;i++)
  {
    vals[i]=rand();
    vals[i]=(vals[i]<<32)|rand();
    vals[i]=vals[i]%(qf.metadata->range);
  }
  double loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  uint64_t insertedItems=0;
  while(loadFactor<0.9){

    qf_insert(&qf,vals[insertedItems],0,50,false,false);
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;
  }
  uint64_t count;
  for(int i=0;i<insertedItems;i++)
  {
    count = qf_count_key_value(&qf, vals[i], 0);
    CHECK(count >= 50);
  }
  QFi qfi;
  qf_iterator(&qf, &qfi, 0);
  do {
    uint64_t key, value, count;
    qfi_get(&qfi, &key, &value, &count);
    count=qf_count_key_value(&qf, key, 0);
    CHECK(count >= 50);
  } while(!qfi_next(&qfi));

  qf_destroy(&qf,true);

}
