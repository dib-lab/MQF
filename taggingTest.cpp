#include "gqf.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include <fstream>
#include "hashutil.h"
#include "kmer.h"
#include <vector>
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

  uint64_t qbits=25;


  uint64_t num_elements=10000000;


  app.add_option("-d,--distribution",distribution,
  "Distributions which items will be drew from. options are zipfian,kmers. Default is zipfian");

  app.add_option("-z",zipifian_coefficient,"Zipifian Coeffiecient. Default =3")->group("Zipfian Distribution Options");
  app.add_option("-u",uniform_coefficient,
  "Frequency of items in the uinform distribution. Default =5")
  ->group("Uniform Distribution Options");
  app.add_option("-k",fqPath,"Fastq file to draw the kmers");

  // app.add_option("-s,--datastructure",dataStrucureInput,
  // "Datastructure to be benchmarked. Options are mqf, lmqf, cqf, and countmin. Default is mqf");

  // app.add_option("-s,--size",qbits
  // ,"No slots equals to 2^(s). default is 25")
  // ->group("Counting Strucutre Options");

  app.add_option("-f,--fpr",fpr
  ,"False positive rate. default is 0.01")
  ->group("Counting Strucutre Options");

  app.add_option("-n,--num_elements",num_elements,
  "Number of elements to generate. Default is 10M");

  app.add_option("-c,--countsError",countErrorMargin
  ,"Counts error margin. default is 5")
  ->group("Countmin options");





  CLI11_PARSE(app, argc, (char**)argv);

  uint64_t p=(uint64_t)log2((double)num_elements/fpr);

  cout<<"Num Hashbits= "<<p<<endl;
  uint64_t estimatedQ=log2((double)num_elements*1.1)+1;// worst case zipifian distribution
  cout<<"Estimated Q= "<<estimatedQ<<endl;

  vector<countingStructure*> dataStructures;

  dataStructures.push_back(new MQF(estimatedQ-1,p-estimatedQ+1,2));



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




  //
  //
  uint64_t countedKmers=0;
  vector<uint64_t> insertions(num_elements);
  //set<uint64_t> succesfullQueries(num_queries);
  uint64_t queryTop=0;

  auto now=std::chrono::high_resolution_clock::now();
  auto prev=std::chrono::high_resolution_clock::now();
  auto microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  uint64_t curr_item;

// check the size of cqf

   uint64_t tmp_inserted=0;
   for(int j=0;j<num_elements;j++){
     if(!g->getElement(curr_item))
       break;
     tmp_inserted++;
  //   cout<<"sss"<<curr_item<<"\n";
     insertions[j]=curr_item;
  //   gold[curr_item]++;
     // if(queryTop<num_queries)
     //    succesfullQueries[queryTop++]=curr_item;
   }
   countedKmers+=num_elements;
   for(auto structure: dataStructures){
     if(structure->space()>=95)
     {
       structure->failed=true;
       continue;
     }
     //  cerr<<"Inserting "<<BufferSize<<" to "<<structure->name<<"("<<structure->space()<<")"<<endl;
     prev=std::chrono::high_resolution_clock::now();

     for(int j=0;j<tmp_inserted;j++){

        structure->insert(ihash(insertions[j],mask),1);
        if(structure->space()>=95)
        {
          structure->failed=true;
          break;
        }

      }
     now=std::chrono::high_resolution_clock::now();
     microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
     microseconds/=1000;
     structure->insertionTime+=microseconds;
   }
  prev=std::chrono::high_resolution_clock::now();
  set<uint64_t> deduplicated(insertions.begin(),insertions.end());
  boophf_t * bphf = new boomphf::mphf<u_int64_t,hasher_t>(deduplicated.size(),deduplicated,1);
  now=std::chrono::high_resolution_clock::now();
  microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  cout<<"Final Number of unique items generated="<<g->nunique_items<<endl;
  QF* mqf=((MQF*)dataStructures[0])->get_MQF();
  qf_ComputeItemsOrder(mqf);
  cout<<"MQF load Factor = "<<qf_space(mqf)<<endl;

  prev=std::chrono::high_resolution_clock::now();
  for(auto i:insertions)
  {
    uint64_t res=bphf->lookup(i);
    
  }
  now=std::chrono::high_resolution_clock::now();
  microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  microseconds/=1000;
  cout<<"MPHF Query Time="<<microseconds<<endl;

  prev=std::chrono::high_resolution_clock::now();
  for(auto i:insertions)
  {
    uint64_t res=itemOrder(mqf,ihash(i,mask));
  }
  now=std::chrono::high_resolution_clock::now();
  microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  microseconds/=1000;
  cout<<"MQF Query Time="<<microseconds<<endl;

  // for(auto structure: dataStructures){
  //   if(structure->failed)
  //   {
  //     structure->fpr=1;
  //     structure->fpr5=1;
  //     structure->fpr10=1;
  //     structure->fpr20=1;
  //     structure->fpr30=1;
  //     continue;
  //
  //   }
  //   cerr<<"Querying  from "<<structure->name<<endl;
  //   prev=std::chrono::high_resolution_clock::now();
  //
  //   for(auto a :g->newItems)
  //   {
  //      uint64_t tmpCount=structure->query(a);
  //
  //      if(tmpCount>0)
  //        structure->fpr++;
  //      if(tmpCount>5)
  //        structure->fpr5++;
  //      if(tmpCount>10)
  //        structure->fpr10++;
  //      if(tmpCount>20)
  //         structure->fpr20++;
  //      if(tmpCount>30)
  //         structure->fpr30++;
  //   }
  //   uint64_t numQueries5=0,
  //            numQueries10=0,
  //            numQueries20=0,
  //            numQueries30=0;
  //
  //   // for(auto it=gold.begin();it!=gold.end();it++)
  //   // {
  //   //   uint64_t tmpCount=structure->query(it->first);
  //   //   if(it->second<5){
  //   //     numQueries5++;
  //   //     if(tmpCount>5)
  //   //       structure->fpr5++;
  //   //   }
  //   //   if(it->second<10){
  //   //     numQueries10++;
  //   //     if(tmpCount>10)
  //   //       structure->fpr10++;
  //   //   }
  //   //   if(it->second<20){
  //   //     numQueries20++;
  //   //     if(tmpCount>20)
  //   //       structure->fpr20++;
  //   //   }
  //   //   if(it->second<30){
  //   //     numQueries30++;
  //   //     if(tmpCount>30)
  //   //       structure->fpr30++;
  //   //   }
  //   //
  //   // }
  //   // now=std::chrono::high_resolution_clock::now();
  //   // structure->fpr/=double(g->newItems.size());
  //   // structure->fpr5/=double(g->newItems.size()+numQueries5);
  //   // structure->fpr10/=double(g->newItems.size()+numQueries10);
  //   // structure->fpr20/=double(g->newItems.size()+numQueries20);
  //   // structure->fpr30/=double(g->newItems.size()+numQueries30);
  //
  //   microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  //   microseconds/=1000;
  //   structure->queryTime+=microseconds;
  //
  //
  // }
  return 0;
}
