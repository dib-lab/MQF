#include "gqf.h"
#include "onDiskMQF.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include "catch.hpp"
#include <stxxl/io>
#include <stxxl/vector>
#include <stxxl/stream>
using namespace std;


using namespace onDiskMQF_Namespace;

const uint64_t MemSize=50;

TEST_CASE( "simple counting test(onDisk)","[onDisk] ") {
  //except first item is inserted 5 times to full test _insert1
  onDiskMQF* qf;
  int counter_size=2;
  uint64_t qbits=5;
  uint64_t diskQbits=6;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  uint64_t count,fixed_counter;
  INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
  onDiskMQF::init(qf ,(1ULL<<diskQbits), num_hash_bits, 0,counter_size, "tmp.ser");
//  qf=new  onDiskMQF_Namespace::_onDiskMQF<10>((1ULL<<qbits),num_hash_bits,0,counter_size,"tmp.ser");

  for(uint64_t i=0;i<=10;i++){
    qf->insert(100,1,false,false);
    count = qf->count_key(100);
    //fixed_counter=qf_get_fixed_counter(&qf,100);
    INFO("Counter = "<<count<<" fixed counter = "<<fixed_counter)
    CHECK(count == (1+i));
  }


  qf->insert(1500,50,false,false);

  count = qf->count_key(1500);
  //  fixed_counter=qf_get_fixed_counter(&qf,1500);
  INFO("Counter = "<<count<<" fixed counter = "<<fixed_counter)
  CHECK(count == (50));

  qf->insert(1600,60,false,false);
  count = qf->count_key(1600);
  //  fixed_counter=qf_get_fixed_counter(&qf,1600);
  INFO("Counter = "<<count<<" fixed counter = "<<fixed_counter)
  CHECK(count == (60));


  qf->insert(2000,4000,false,false);
  count = qf->count_key(2000);
  //  fixed_counter=qf_get_fixed_counter(&qf,2000);
  INFO("Counter = "<<count<<" fixed counter = "<<fixed_counter)
  CHECK(count == (4000));

  delete qf;

}

TEST_CASE( "Maximum count(onDisk)","[onDisk]" ) {
  onDiskMQF* qf;
  int counter_size=4;
  srand (1);
  uint64_t qbits=5;
  uint64_t diskQbits=6;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
   INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
   onDiskMQF::init(qf, (1ULL<<qbits), num_hash_bits, 0,counter_size ,"tmp.ser");
  qf->metadata->maximum_count=10;
  qf->insert(100,100000,false,false);
  uint64_t count = qf->count_key(100);
  CHECK(count==10);

  qf->insert(150,8,false,false);
  qf->insert(150,8,false,false);
  count = qf->count_key(150);
  CHECK(count==10);
    delete qf;

}
// //
 TEST_CASE( "Big count(onDisk)","[onDisk]" ) {
   onDiskMQF* qf;
   int counter_size=4;
   srand (1);
   uint64_t qbits=5;
   uint64_t diskQbits=6;
   uint64_t num_hash_bits=qbits+8;
   uint64_t maximum_count=(1ULL<<counter_size)-1;
   INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
   onDiskMQF::init(qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, "tmp.ser");
   qf->insert(100,100000,false,false);
   uint64_t count = qf->count_key(100);

   CHECK(count==100000);
   delete qf;

 }

TEST_CASE( "Inserting items( repeated 1 time) in cqf(90% load factor )(onDisk)" ,"[onDisk]") {
  //except first item is inserted 5 times to full test _insert1
  onDiskMQF* qf;
  int counter_size=2;
  uint64_t qbits=15;
  uint64_t num_hash_bits=qbits+9;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
  onDiskMQF::init(qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, "tmp.ser");
  uint64_t nvals = (1ULL<<qbits)*2;
  uint64_t *vals;
  vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));
  for(uint64_t i=0;i<nvals;i++)
  {
    vals[i]=rand();
    vals[i]=(vals[i]<<32)|rand();
    vals[i]=vals[i]%(qf->metadata->range);
  }

  int loadFactor=qf->space();;
  uint64_t insertedItems=0;
//  onDiskMQF_dump(&qf);

    qf->insert(vals[0],1,false,false);
    qf->insert(vals[0],1,false,false);
    qf->insert(vals[0],1,false,false);
    qf->insert(vals[0],1,false,false);
  // for(uint64_t i=0;i<32;i++)
  // {
  //   cout<<get_fixed_counter(&qf,i)<<"-";
  // }
  //cout<<endl;
  uint64_t count;
  while(loadFactor<90){


    qf->insert(vals[insertedItems],1,false,false);

    //onDiskMQF_dump(&qf);
    count = qf->count_key(vals[insertedItems]);
    CHECK(count >= 1);
    // for(uint64_t i=0;i<32;i++)
    // {
    //   cout<<get_fixed_counter(&qf,i)<<"-";
    // }
    // cout<<endl;
    insertedItems++;
    loadFactor=qf->space();
    //cout<<"loadFactor "<<loadFactor<<endl;
  }
  INFO("Inserted Items = "<<insertedItems);


  //INFO("Fixed counter = "<<qf_get_fixed_counter(&qf,vals[0]));
  count = qf->count_key(vals[0]);
  CHECK(count >= 5);

  for(uint64_t i=1;i<insertedItems;i++)
  {
    count = qf->count_key(vals[i]);
    CHECK(count >= 1);
  }
   onDiskMQFIterator qfi;
   qf->getIterator( &qfi, 0);
   do {
     uint64_t key, value, count;
     qfi.get(&key, &value, &count);
     if(key==vals[0]){
       CHECK(count >= 5);
     }
     else{
       CHECK(count >= 1);
     }

   } while(qfi.next());
    delete qf;
//  onDiskMQF_destroy(&qf);

}
//
//
TEST_CASE( "Inserting items( repeated 50 times) in cqf(90% load factor ) (onDisk)" ,"[onDisk] " ) {
  onDiskMQF* qf;
  int counter_size=4;
  uint64_t qbits=15;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
  onDiskMQF::init(qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, "tmp.ser");

  uint64_t nvals = (1ULL<<qbits);
  uint64_t *vals;
  vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));
  for(uint64_t i=0;i<nvals;i++)
  {
    vals[i]=rand();
    vals[i]=(vals[i]<<32)|rand();
    vals[i]=vals[i]%(qf->metadata->range);
  }
  double loadFactor=(double)qf->metadata->noccupied_slots/(double)qf->metadata->nslots;
  uint64_t insertedItems=0;
  uint64_t count;
  while(loadFactor<0.9){
    qf->insert(vals[insertedItems],50,false,false);

    insertedItems++;
    loadFactor=(double)qf->metadata->noccupied_slots/(double)qf->metadata->nslots;
  }

  for(uint64_t i=0;i<insertedItems;i++)
  {
    count = qf->count_key(vals[i]);
    CHECK(count >= 50);
  }
  delete qf;
  // onDiskMQFIterator qfi;
  // onDiskMQF_iterator(&qf, &qfi, 0);
  // do {
  //   uint64_t key, value, count;
  //   qfi_get(&qfi, &key, &value, &count);
  //   count=qf->count_key(key);
  //   CHECK(count >= 50);
  // } while(!qfi_next(&qfi));

//  onDiskMQF_destroy(&qf);

}
//
//
TEST_CASE( "copy (onDisk)","[onDisk][!hide]" ) {
  onDiskMQF* qf,*qf2;
  int counter_size=4;
  srand (1);
  uint64_t qbits=15;
  uint64_t diskQbits=16;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
  onDiskMQF::init(qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, "tmpcp.ser");
  onDiskMQF::init(qf2, (1ULL<<qbits), num_hash_bits, 0,counter_size, "tmp5.ser");
  uint64_t nvals = (1ULL<<diskQbits);
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
      newvalue=newvalue%(qf->metadata->range);
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


    nRepetitions[i]=(rand()%1000)+1;
  }
  int loadFactor=qf->space();
  uint64_t insertedItems=0;
  while(insertedItems<nvals && loadFactor<90){
  //  printf("inserting %lu count = %lu\n",vals[insertedItems],nRepetitions[insertedItems] );
    INFO("Inserting "<< vals[insertedItems] << " Repeated "<<nRepetitions[insertedItems]);
    qf->insert(vals[insertedItems],nRepetitions[insertedItems],false,false);
    //qf_dump(&qf);
    INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
    count = qf->count_key(vals[insertedItems]);
    CHECK(count == nRepetitions[insertedItems]);
    insertedItems++;
    loadFactor=qf->space();

  }
  INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
  qf->copy(qf2);
  for(uint64_t i=0;i<insertedItems;i++)
  {
    count = qf2->count_key(vals[i]);
    INFO("value = "<<vals[i]<<" Repeated " <<nRepetitions[i]);
    REQUIRE(count == nRepetitions[i]);
  }
  delete qf,qf2;
  //onDiskMQF_destroy(&qf);

}

TEST_CASE( "Inserting items( repeated 1-1000 times) in cqf(90% load factor )(onDisk)","[onDisk] " ) {
  onDiskMQF* qf;
  int counter_size=4;
  srand (1);
  uint64_t qbits=15;
  uint64_t diskQbits=16;
  uint64_t num_hash_bits=qbits+8;
  uint64_t maximum_count=(1ULL<<counter_size)-1;
  INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
  onDiskMQF::init(qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, "tmp.ser2");

  uint64_t nvals = (1ULL<<diskQbits);
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
      newvalue=newvalue%(qf->metadata->range);
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


    nRepetitions[i]=(rand()%1000)+1;
  }
  int loadFactor=qf->space();
  uint64_t insertedItems=0;
  while(insertedItems<nvals && loadFactor<90){
  //  printf("inserting %lu count = %lu\n",vals[insertedItems],nRepetitions[insertedItems] );
    INFO("Inserting "<< vals[insertedItems] << " Repeated "<<nRepetitions[insertedItems]);
    qf->insert(vals[insertedItems],nRepetitions[insertedItems],false,false);
    //qf_dump(&qf);
    INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
    count = qf->count_key(vals[insertedItems]);
    CHECK(count == nRepetitions[insertedItems]);
    insertedItems++;
    loadFactor=qf->space();

  }
  INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);

  for(uint64_t i=0;i<insertedItems;i++)
  {
    count = qf->count_key(vals[i]);
    INFO("value = "<<vals[i]<<" Repeated " <<nRepetitions[i]);
    CHECK(count == nRepetitions[i]);
  }
    delete qf;
 // onDiskMQF_destroy(&qf);

}


 TEST_CASE( "Migrate(onDisk)","[onDisk] " ) {
   onDiskMQF *qf,*qf2;
   int counter_size=4;
   srand (1);
   uint64_t qbits=16;
   uint64_t num_hash_bits=qbits+8;
   uint64_t maximum_count=(1ULL<<counter_size)-1;
   INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
   onDiskMQF::init(qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, "tmp.ser22");
   onDiskMQF::init(qf2, (1ULL<<qbits), num_hash_bits, 0,counter_size, "tmp.ser33");

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
       newvalue=newvalue%(qf->metadata->range);
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


     nRepetitions[i]=(rand()%1000)+1;
   }
   double loadFactor=(double)qf->metadata->noccupied_slots/(double)qf->metadata->nslots;
   uint64_t insertedItems=0;
   while(insertedItems<nvals && loadFactor<0.9){
   //  printf("inserting %lu count = %lu\n",vals[insertedItems],nRepetitions[insertedItems] );
     INFO("Inserting "<< vals[insertedItems] << " Repeated "<<nRepetitions[insertedItems]);
     qf2->insert(vals[insertedItems],nRepetitions[insertedItems],false,false);
     //qf_dump(&qf);
     INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
     count = qf2->count_key( vals[insertedItems]);
     CHECK(count == nRepetitions[insertedItems]);
     insertedItems++;
     loadFactor=(double)qf2->metadata->noccupied_slots/(double)qf->metadata->nslots;

   }
   INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
   qf2->migrate(qf);
   for(uint64_t i=0;i<insertedItems;i++)
   {
     count = qf->count_key(vals[i]);
     INFO("value = "<<vals[i]<<" Repeated " <<nRepetitions[i]);
     CHECK(count == nRepetitions[i]);
   }

   delete qf,qf2;

 }
//
 TEST_CASE( "Counting Big counters(onDisk)","[onDisk]" ){
   onDiskMQF* qf;
   int counter_size=2;
   srand (1);
   uint64_t qbits=16;
   uint64_t num_hash_bits=qbits+8;
   uint64_t maximum_count=(1ULL<<counter_size)-1;
   INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
   onDiskMQF::init(qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, "tmp.ser2");


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
     vals[i]=vals[i]%(qf->metadata->range);

     nRepetitions[i]=(rand())+1;
   }
   double loadFactor=(double)qf->metadata->noccupied_slots/(double)qf->metadata->nslots;
   uint64_t insertedItems=0;
   while(insertedItems<nvals && loadFactor<0.9){
   //  printf("inserting %lu count = %lu\n",vals[insertedItems],nRepetitions[insertedItems] );
     INFO("Inserting "<< vals[insertedItems] << " Repeated "<<nRepetitions[insertedItems]);
     qf->insert(vals[insertedItems],nRepetitions[insertedItems],false,false);
     //onDiskMQF_dump(&qf);
     INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
     count = qf->count_key(vals[insertedItems]);
     CHECK(count >= nRepetitions[insertedItems]);
     insertedItems++;
     loadFactor=(double)qf->metadata->noccupied_slots/(double)qf->metadata->nslots;

   }
   INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);

   for(uint64_t i=0;i<insertedItems;i++)
   {
     count = qf->count_key(vals[i]);
     INFO("value = "<<vals[i]<<" Repeated " <<nRepetitions[i]);
     CHECK(count >= nRepetitions[i]);
   }

   delete qf;


 }
//
 TEST_CASE( "Removing items from cqf(90% load factor )(onDisk)","[onDisk] ") {
   onDiskMQF* qf;
   int counter_size=2;
   uint64_t qbits=16;
   uint64_t num_hash_bits=qbits+8;
   uint64_t maximum_count=(1ULL<<counter_size)-1;

   onDiskMQF::init(qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, "tmp.ser2");
   uint64_t nvals = (1ULL<<qbits);
   uint64_t *vals;
   vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));
   for(uint64_t i=0;i<nvals;i++)
   {
     uint64_t newvalue=0;
     while(newvalue==0){
       newvalue=rand();
       newvalue=(newvalue<<32)|rand();
       newvalue=newvalue%(qf->metadata->range);
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
   double loadFactor=(double)qf->metadata->noccupied_slots/(double)qf->metadata->nslots;
   uint64_t insertedItems=0;
   while(loadFactor<0.9){

     qf->insert(vals[insertedItems],50,false,false);
     insertedItems++;
     loadFactor=(double)qf->metadata->noccupied_slots/(double)qf->metadata->nslots;
   }
   uint64_t count;
   for(uint64_t i=0;i<insertedItems;i++)
   {
     if(i%2==0){
       count = qf->count_key(vals[i]);
       if(count==100){
         printf("coubn ==100\n" );
       }
     qf->remove(vals[i],50,false,false);
     count = qf->count_key(vals[i]);
     CHECK(count ==0);
     }
   }
   for(uint64_t i=0;i<insertedItems;i++)
   {
     count = qf->count_key(vals[i]);
     if(i%2==1){
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
   onDiskMQFIterator qfi;
   qf->getIterator( &qfi, 0);
   do {
     uint64_t key, value, count;
     qfi.get(&key, &value, &count);
     //count=qf->count_key(key);
     CHECK(count >= 50);
   } while(!qfi.next());

    delete qf;

 }

TEST_CASE( "test get_iterator (onDisk)","[onDisk]" ) {
    onDiskMQF* qf;
    int counter_size=2;
    uint64_t qbits=16;
    uint64_t num_hash_bits=qbits+8;
    uint64_t maximum_count=(1ULL<<counter_size)-1;

    onDiskMQF::init(qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, "tmp.ser2");

    uint64_t nvals = (1ULL<<qbits);
    //nvals = 5000;
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
            newvalue=newvalue%(qf->metadata->range);
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


        nRepetitions[i]=(rand()%1000)+1;
    }
    double loadFactor=(double)qf->metadata->noccupied_slots/(double)qf->metadata->nslots;
    uint64_t insertedItems=0;
    while(insertedItems<nvals && loadFactor<0.9){
        //  printf("inserting %lu count = %lu\n",vals[insertedItems],nRepetitions[insertedItems] );
        INFO("Inserting "<< vals[insertedItems] << " Repeated "<<nRepetitions[insertedItems]);
        qf->insert(vals[insertedItems],nRepetitions[insertedItems],false,false);
        INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
        count = qf->count_key(vals[insertedItems]);
        //  CHECK(count == nRepetitions[insertedItems]);
        insertedItems++;
        loadFactor=(double)qf->metadata->noccupied_slots/(double)qf->metadata->xnslots;

    }
    INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);

    for(uint64_t i=0;i<insertedItems/10;i++)
    {
        onDiskMQFIterator it;
        bool res = qf->findIterator(&it, vals[i]);
        uint64_t key,value,count,newkey;
        it.get(&key,&value,&count);


        INFO("value = "<<vals[i]<<" Repeated " <<nRepetitions[i]);
        CHECK(count == nRepetitions[i]);


    }


}


TEST_CASE( "Writing and Reading to/from Disk (onDisk)","[onDisk]" ) {
  class st{
  public:
      int a;
      int b;
      st(int aa,int bb)
      {
        a=aa;
        b=bb;
      }
      st(){
        a=0;
        b=0;
      }
  };
  string output_path="tmptest";
  using stxxl::file;
  file::unlink(output_path.c_str()); // delete output file
  stxxl::timer tm(true); // start a timer
  // input file object
  //stxxl::syscall_file InputFile(input_path, file::RDONLY | file::DIRECT);
  // output file object
  stxxl::syscall_file OutputFile(output_path, file::RDWR | file::CREAT | file::DIRECT);
  typedef stxxl::vector<st> vector_type;

  // InputVector is mapped to InputFile
  //vector_type InputVector(&InputFile);
  vector_type InputVector(10);
  vector_type OutputVector(&OutputFile,262784, 1); // OutputVector is mapped to OutputFile
  // std::cout << "File " << input_path << " has size " << InputVector.size() << " bytes." << std::endl;

  for(int i=0;i<1000000;i++)
    OutputVector.push_back(st(i,i+1));

  OutputVector.flush();
//    for(int i=0;i<1000;i++)
//        cout<<InputVector[i].a<<" "<<InputVector[i].b<<endl;

  OutputVector.export_files("/home/mostafa/Desktop/tmp");



    onDiskMQF* qf;
    int counter_size=2;
    uint64_t qbits=24;
    uint64_t num_hash_bits=qbits+8;
    uint64_t maximum_count=(1ULL<<counter_size)-1;
    INFO("Counter size = "<<counter_size<<" max count= "<<maximum_count);
    onDiskMQF::init(qf, (1ULL<<qbits), num_hash_bits, 0,counter_size, "tmp.ser2");

    //uint64_t nvals = (1ULL<<qbits);
    uint64_t nvals = (1ULL<<10);
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
            newvalue=newvalue%(qf->metadata->range);
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
    double loadFactor=(double)qf->metadata->noccupied_slots/(double)qf->metadata->nslots;
    uint64_t insertedItems=0;
    while(insertedItems<nvals && loadFactor<0.7){
        qf->insert(vals[insertedItems],nRepetitions[insertedItems],false,false);

        count = qf->count_key( vals[insertedItems]);
        CHECK(count >= nRepetitions[insertedItems]);
        insertedItems++;
        loadFactor=(double)qf->metadata->noccupied_slots/(double)qf->metadata->nslots;
    }
    INFO("Load factor = "<<loadFactor <<" inserted items = "<<insertedItems);
    INFO("nslots ="<<qf->metadata->nslots);
    qf->serialize();
    delete qf;


    //onDiskMQF* qf2;
//    onDiskMQF::load(qf2,"tmp.ser2");
//    INFO("nslots ="<<qf2->metadata->nslots);
//    for(uint64_t i=0;i<insertedItems;i++)
//    {
//        count = qf2->count_key(vals[i]);
//        INFO("value = "<<vals[i]<<" Repeated " <<nRepetitions[i]);
//        REQUIRE(count >= nRepetitions[i]);
//
//    }
//
//    delete qf2;





}