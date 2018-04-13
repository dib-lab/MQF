
#include "../catch.hpp"

TEST_CASE( "get/set fixed counters" ) {
  QF qf;
  for(uint64_t counter_size=1;counter_size<=5;counter_size++){
    uint64_t qbits=15;
    uint64_t num_hash_bits=qbits+8;
    uint64_t maximum_count=(1ULL<<counter_size)-1;
    INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
    qf_init(&qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, true, "", 2038074761);
    uint64_t c;
    // test many set and get
    uint64_t last=0;
    for(uint64_t i=1;i<=maximum_count;i++){
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
    for(uint64_t i=0;i<3;i++){
      INFO("Testing Special Slot "<<special_slots[i]);
      REQUIRE( get_fixed_counter(&qf,special_slots[i]) == 0 );
      set_fixed_counter(&qf,special_slots[i],c);
      REQUIRE( get_fixed_counter(&qf,special_slots[i]) == c );
    }

    qf_destroy(&qf);
  }
}

TEST_CASE( "shift fixed counters" ) {
  QF qf;
  for(uint64_t counter_size=1;counter_size<=5;counter_size++){
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

    qf_destroy(&qf);
  }
}


TEST_CASE( "get/set Tags" ) {
  QF qf;
  for(uint64_t tag_size=1;tag_size<=5;tag_size++){
    uint64_t qbits=7;
    uint64_t num_hash_bits=qbits+8;
    uint64_t maximum_count=(1ULL<<tag_size)-1;
    INFO("Tag size = "<<tag_size);
    qf_init(&qf, (1ULL<<qbits), num_hash_bits, tag_size,3, true, "", 2038074761);
    uint64_t c;
    // test many set and get
    uint64_t last=0;
    for(uint64_t i=1;i<=maximum_count;i++){
      REQUIRE( get_tag(&qf,99) == 0 );
      CHECK( get_tag(&qf,100) == last );
      REQUIRE( get_tag(&qf,101) == 0 );
      set_tag(&qf,100,i);
      REQUIRE( get_tag(&qf,99) == 0 );
      CHECK( get_tag(&qf,100) == i );
      REQUIRE( get_tag(&qf,101) == 0 );
      last=i;
    }

    //test on block boundaries

    REQUIRE( get_tag(&qf,63) == 0 );
    REQUIRE( get_tag(&qf,64) == 0 );
    REQUIRE( get_tag(&qf,65) == 0 );

    c=1;
    set_tag(&qf,63,c);
    c=(c+1)%maximum_count;
    set_tag(&qf,64,c);
    c=(c+1)%maximum_count;
    set_tag(&qf,65,c);

    c=1;
    REQUIRE( get_tag(&qf,63) == c );
    c=(c+1)%maximum_count;
    REQUIRE( get_tag(&qf,64) == c );
    c=(c+1)%maximum_count;
    REQUIRE( get_tag(&qf,65) == c );

    // test special slots
    c=1;
    uint64_t special_slots[]={
      0,
      (1ULL<qbits),
      qf.metadata->xnslots-1
    };
    for(uint64_t i=0;i<3;i++){
      INFO("Testing Special Slot "<<special_slots[i]);
      REQUIRE( get_tag(&qf,special_slots[i]) == 0 );
      set_tag(&qf,special_slots[i],c);
      REQUIRE( get_tag(&qf,special_slots[i]) == c );
    }

    qf_destroy(&qf);
  }
}

TEST_CASE("testing copy2") {
  QF qf;
  int counter_size=2;
  srand (1);
  uint64_t qbits=15;
  uint64_t num_hash_bits=qbits+9;
  uint64_t tag_size=5;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  uint64_t maximum_tag=(1ULL<<tag_size)-1;

  INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
  qf_init(&qf, (1ULL<<qbits), num_hash_bits, tag_size,counter_size, true, "", 2038074761);

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
    qf_add_tag(&qf,vals[insertedItems],vals[insertedItems]%(maximum_tag+1));

    //qf_dump(&qf);
    INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
    count = qf_count_key(&qf, vals[insertedItems]);
    CHECK(count >= nRepetitions[insertedItems]);
    insertedItems++;
    loadFactor=(double)qf.metadata->noccupied_slots/(double)qf.metadata->nslots;

  }
  INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);

  QF* qf2=(QF*)calloc(sizeof(QF),1);
  qf_init(qf2, (1ULL<<qbits), num_hash_bits, tag_size-3,counter_size, true, "", 2038074761);

  qf_copy2(qf2,&qf);

  qf_destroy(&qf);
  for(uint64_t i=0;i<insertedItems;i++)
  {
    count = qf_count_key(qf2, vals[i]);
    INFO("value = "<<vals[i]<<" Repeated " <<nRepetitions[i]);
    CHECK(count >= nRepetitions[i]);

    count = qf_get_tag(qf2,vals[i]);
    uint64_t newcount=vals[i]%(maximum_tag+1);
    newcount>>=3;

    CHECK(count == newcount);
  }

  qf_destroy(qf2);

}
