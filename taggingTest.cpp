#include "gqf.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include <fstream>
#include "hashutil.h"
#include "kmer.h"
#include <vector>
#include <deque>
#include <chrono>
#include<cmath>
#include <random>
#include <algorithm>
#include "Benchmark_Utils_Script/generators.hpp"
#include "Benchmark_Utils_Script/countingStructure.hpp"
#include "CLI11.hpp"
#include "countmin-khmer/storage.hh"
#include "BBHash/BooPHF.h"

using namespace std;

#include <iostream>


typedef boomphf::SingleHashFunctor<u_int64_t>  hasher_t;
typedef boomphf::mphf<  u_int64_t, hasher_t  > boophf_t;

int main(int argc, char const *argv[]) {

  cout<<"Command = ";
  for(int i=0;i<argc;i++)
  {
    cout<<argv[i]<<" ";
  }
  cout<<endl;

  CLI::App app;
  string distribution="zipfian";
  double zipifian_coefficient=3.0;
  double uniform_coefficient=5;
  string dataStrucureInput="mqf";
  double fpr=0.01;
  uint64_t countErrorMargin=5;
  uint64_t kSize=25;
  string fqPath="";
  uint64_t Q=0;
  uint64_t qbits=25;
  uint64_t loadFactor=60;

  uint64_t num_elements=100000000;
  //uint64_t num_elements=0;

  app.add_option("-d,--distribution",distribution,
  "Distributions which items will be drew from. options are zipfian,kmers. Default is zipfian");

  app.add_option("-z",zipifian_coefficient,"Zipifian Coeffiecient. Default =3")->group("Zipfian Distribution Options");
  app.add_option("-u",uniform_coefficient,
  "Frequency of items in the uinform distribution. Default =5")
  ->group("Uniform Distribution Options");
  app.add_option("-k",fqPath,"Fastq file to draw the kmers");
  app.add_option("-q",Q,"MQF Q");
  app.add_option("-l",loadFactor,"MQF Load Factor");
  // app.add_option("-s,--datastructure",dataStrucureInput,
  // "Datastructure to be benchmarked. Options are mqf, lmqf, cqf, and countmin. Default is mqf");

  // app.add_option("-s,--size",qbits
  // ,"No slots equals to 2^(s). default is 25")
  // ->group("Counting Strucutre Options");

  app.add_option("-f,--fpr",fpr
  ,"False positive rate. default is 0.01")
  ->group("Counting Strucutre Options");

  //app.add_option("-n,--num_elements",num_elements,
  //"Number of elements to generate. Default is 10M");

  app.add_option("-c,--countsError",countErrorMargin
  ,"Counts error margin. default is 5")
  ->group("Countmin options");





  CLI11_PARSE(app, argc, (char**)argv);

//  uint64_t p=(uint64_t)log2((double)num_elements*1.6/fpr);
  uint64_t p=kSize*2;
  cout<<"Num Hashbits= "<<p<<endl;
//  uint64_t estimatedQ=log2((double)num_elements*1.2)+1;// worst case zipifian distribution
//  if(Q==0)
//    Q=estimatedQ;
  //cout<<"Estimated Q= "<<estimatedQ<<endl;
  cout<<"Used Q= "<<Q<<endl;
  MQF* MQFCS= new MQF(Q,p-Q,2);



  uint64_t range=(1ULL<<(int)p);
  srand (1);

  map<uint64_t,uint64_t> gold;

  uint64_t mask=BITMASK(2*kSize);

  generator *g;
  if(distribution=="uniform")
  {
    g=new uniformGenerator(num_elements,range,uniform_coefficient);
  }
  else if(distribution=="kmers")
  {
    g= new kmersGenerator(num_elements,fqPath,kSize);
  }
  else if(distribution=="zipfian"){
    g= new zipfGenerator(num_elements,range,zipifian_coefficient);
  }
  else{
    cout<<"Unknown Distribution"<<endl;
    return -1;
  }
  uint64_t BufferSize=num_elements/25;



  //
  //
  uint64_t countedKmers=0;
  set<uint64_t> insertions;
  //insertions.reserve(num_elements);

  uint64_t tmp_inserted=0;
  uint64_t curr_item;
  while(MQFCS->space()<loadFactor){
     if(!g->getElement(curr_item))
       break;

     MQFCS->insert(ihash(curr_item,mask),1);
     if(MQFCS->space()>=95)
     {
       MQFCS->failed=true;
       cout<<"Failed"<<endl;
       break;
     }
     insertions.insert(curr_item);
     tmp_inserted++;
  }
  cout<<"Generating "<<insertions.size()<<" items"<<endl;
  uint64_t queryTop=0;


  countedKmers+=num_elements;
     //  cerr<<"Inserting "<<BufferSize<<" to "<<structure->name<<"("<<structure->space()<<")"<<endl;

  auto prev=std::chrono::high_resolution_clock::now();
  QF* mqf=MQFCS->get_MQF();
  qf_ComputeItemsOrder(mqf);
  auto now=std::chrono::high_resolution_clock::now();
  auto microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  MQFCS->insertionTime=microseconds;

  prev=std::chrono::high_resolution_clock::now();
  //set<uint64_t> dedpuplicated(insertions.begin(),insertions.end());
  boophf_t * bphf = new boomphf::mphf<u_int64_t,hasher_t>(insertions.size(),insertions,1);
  //boophf_t * bphf = new boomphf::mphf<u_int64_t,hasher_t>(dedpuplicated.size(),dedpuplicated,1);
//  dedpuplicated.clear();
  now=std::chrono::high_resolution_clock::now();
  microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  cout<<"MPHF Construction Time = "<<microseconds<<"microsecons"<<endl;



  cout<<"MQF load Factor = "<<qf_space(mqf)<<endl;
  vector<uint64_t> queries(insertions.begin(),insertions.end());
  insertions.clear();
  prev=std::chrono::high_resolution_clock::now();
  uint64_t numQueries=35000000;
  for(uint64_t i=0;i<numQueries;i++)
  {

    uint64_t res=bphf->lookup(queries[i]);
  }
  now=std::chrono::high_resolution_clock::now();
  microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  microseconds/=1000;
  uint64_t mphf_bytes=bphf->totalBitSize()/8;
  cout<<"MPHF Size="<<mphf_bytes/(1024*1024)<<"MB"<<endl;
  cout<<"MPHF Query Time="<<microseconds<<endl;

  prev=std::chrono::high_resolution_clock::now();
  for(uint64_t i=0;i<numQueries;i++)
  {
    uint64_t res=itemOrder(mqf,ihash(queries[i],mask));
  }
  now=std::chrono::high_resolution_clock::now();
  microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  microseconds/=1000;
  MQFCS->queryTime=microseconds;

  cout<<"Name"<<"\t"
      <<"Size"<<"\t"
      <<"Space"<<"\t"
      <<"fpr"<<"\t"
      <<"Insertion Speed"<<"\t"
      <<"Query Speed"<<endl;
  MQFCS->print();
  cout<<"MQF Query Time="<<microseconds<<endl;

  return 0;
}
