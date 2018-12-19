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

using namespace std;

#include <iostream>




int main(int argc, char const *argv[]) {
  CLI::App app;
  string distribution="zipfian";
  double zipifian_coefficient=3.0;
  double uniform_coefficient=5;
  string dataStrucureInput="mqf";


  uint64_t fixedCounterSize=2;

  uint64_t qbits=25;
  uint64_t kSize=20;
  string fqPath="";
  uint64_t num_elements=0;
  double fp_rate=0.0001;
  int numberOfTables=4;
  app.add_option("-d,--distribution",distribution,
  "Distributions which items will be drew from. options are uniform,zipfian. Default is zipfian");
   app.add_option("-n,--num_elements",num_elements,
   "Number of elements to generate(0 means infinity)");
  app.add_option("-z",zipifian_coefficient,"Zipifian Coeffiecient. Default =3")->group("Zipfian Distribution Options");
  app.add_option("-u",uniform_coefficient,
  "Frequency of items in the uinform distribution. Default =5")
  ->group("Uniform Distribution Options");
  // app.add_option("-s,--datastructure",dataStrucureInput,
  // "Datastructure to be benchmarked. Options are mqf, lmqf, cqf, and countmin. Default is mqf");
  app.add_option("-k",fqPath,"Fastq file to draw the kmers");

  app.add_option("-s,--size",qbits
  ,"No slots equals to 2^(s). default is 25")
  ->group("Counting Strucutre Options");
  app.add_option("-f,--fp-rate",fp_rate
  ,"False Positive rate. default is 0.0001")
  ->group("Counting Strucutre Options");
  app.add_option("-t,--num-tables",numberOfTables
  ,"Number of tables in countmin sketch. default is 4")
  ->group("Counting Strucutre Options");





  CLI11_PARSE(app, argc, (char**)argv);





  // if(distribution=="kmers")
  // {
  //   slot_size=kSize*2-qbits;
  // }
  num_elements=(1ULL<<qbits)*1.2;
  //num_elements=6053636;
  int p=int(ceil(log2((float)(num_elements)/fp_rate)));
  if(p>2*kSize)
    p=2*kSize;
  cout<<"P = "<<p<<endl;
  uint64_t slot_size=p-qbits;
  uint64_t BufferSize=num_elements/40;
  //  uint64_t num_queries=num_elements/8;
  uint64_t num_queries=2500000;
  vector<countingStructure*> dataStructures;
  //double p=(double)(slot_size+qbits);
  dataStructures.push_back(new MQF(qbits,slot_size,fixedCounterSize));
  dataStructures.push_back(new CQF(qbits,slot_size));
  cout<<"Number of elements = "<<num_elements<<endl;
  cout<<"CQF/MQFF Q = "<<qbits<<endl;
  cout<<"CQF/MQF slot size= "<<slot_size<<endl;
  cout<<"CQF/MQF num hashbits= "<<p<<endl;

  //int countMinDepth=(p*log(2.0)-log((double)num_elements))+1;
  int countMinDepth=int(ceil(log(1.0/fp_rate)));
  uint64_t countMinWidth = dataStructures[0]->size / countMinDepth;
  double e=2.71828;
//  uint64_t countMinWidth=(e*99824410)+1;
  cout<<"Count Min Sketch1 Width= "<<countMinWidth<<endl;
  cout<<"Count Min Sketch1 Depth= "<<countMinDepth<<endl;
  //
  dataStructures.push_back(new countmin(countMinWidth,countMinDepth));
  dataStructures.push_back(new countminKhmer(countMinWidth,countMinDepth));
  // countMinWidth=(e*num_elements);
  // countMinDepth=numberOfTables;
  // double new_fp_rate = pow(1 - exp(-double(num_elements) / double(countMinWidth)) , countMinDepth);
  // // while(new_fp_rate>fp_rate){
  // //   new_fp_rate = pow(1 - exp(-double(num_elements) / double(countMinWidth)) , countMinDepth);
  // //   countMinWidth+=1000;
  // // //  cout<<new_fp_rate<<endl;
  // // }
  // cout<<new_fp_rate<<endl;
  // cout<<"Count Min Sketch2 Width= "<<countMinWidth<<endl;
  // cout<<"Count Min Sketch2 Depth= "<<countMinDepth<<endl;
  // dataStructures.push_back(new countmin(countMinWidth,countMinDepth));
  // dataStructures.push_back(new countminKhmer(countMinWidth,countMinDepth));
  //


  //dataStructure.push_back(new LMQF(singleQbits,qbits,slot_size,fixedCounterSize));
  dataStructures.push_back(new BMQF(qbits,qbits-2,slot_size,fixedCounterSize));
  cout<<"Buffered MQF buffer Q = "<<qbits-2<<endl;
  cout<<"STXXL buffer size= "<<((BMQF*)dataStructures.back())->bsize<<"MB"<<endl;
  uint64_t range=(1ULL<<(int)p);


  srand (1);




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

  cout<<"Command = ";
  for(int i=0;i<argc;i++)
  {
    cout<<argv[i]<<" ";
  }
  cout<<endl;


  //
  //
  uint64_t countedKmers=0;
  vector<uint64_t> insertions(BufferSize);
  vector<uint64_t> succesfullQueries(num_queries);
  uint64_t queryTop=0;
  // auto now = std::chrono::high_resolution_clock::now();
  // auto prev=now;
  // now = std::chrono::high_resolution_clock::now();
  //
  //
  //
  // vector<pair<int,int> > lmqfSpace;
  // vector<pair<double,double> > result;
  // vector<pair<double,double> > results_slotsUsed;
  // cout<<"Capacity\t1M insertions per second"<<endl;
  // double capacity=dataStructure->space();
  //
  //
  // uint64_t old_capacity=capacity;
  // uint64_t insertiontime=0;
  // uint64_t querytime=0;
  // uint64_t curr_item;
  auto now=std::chrono::high_resolution_clock::now();
  auto prev=std::chrono::high_resolution_clock::now();
  auto microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  uint64_t curr_item;
// check the size of cqf
  while(dataStructures[1]->space()<85 && g->hasMore())
   {
     prev=std::chrono::high_resolution_clock::now();
     for(int j=0;j<BufferSize;j++){
       if(!g->getElement(curr_item))
         break;
       insertions[j]=curr_item;
       if(queryTop<num_queries)
          succesfullQueries[queryTop++]=curr_item;
     }
     now=std::chrono::high_resolution_clock::now();
     microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
     cerr<<"Generating items in "<<microseconds<<"microseconds"<<endl;
     countedKmers+=BufferSize;
     for(auto structure: dataStructures){
       cerr<<"Inserting "<<BufferSize<<" to "<<structure->name<<"("<<structure->space()<<")"<<endl;
       prev=std::chrono::high_resolution_clock::now();

       for(int j=0;j<BufferSize;j++){
          structure->insert(insertions[j],1);
        //  cout<<insertions[j]<<endl;
        }

       now=std::chrono::high_resolution_clock::now();
       microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
       microseconds/=1000;
       structure->insertionTime+=microseconds;
     }
  }
  for(auto structure: dataStructures){
    cerr<<"Querying "<<succesfullQueries.size()+g->newItems.size()<<" from "<<structure->name<<endl;
    if(structure->name=="Buffered MQF")
    {
      vector<uint64_t> tmp(g->newItems.size()+succesfullQueries.size());
      auto tmpIT=std::copy(g->newItems.begin(),g->newItems.end(),tmp.begin());
      std::copy(succesfullQueries.begin(),succesfullQueries.end(),tmpIT);
      prev=std::chrono::high_resolution_clock::now();
      //      ((BMQF*)structure)->batchQuery(succesfullQueries);
      ((BMQF*)structure)->batchQuery(tmp);
      now=std::chrono::high_resolution_clock::now();
      microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
      microseconds/=1000;
      structure->queryTime+=microseconds;
    }
    else{
      prev=std::chrono::high_resolution_clock::now();
      for(auto a :succesfullQueries)
      {
         structure->query(a);
      }
      for(auto a :g->newItems)
      {
         uint64_t tmpCount=structure->query(a);
         if(tmpCount>9)
         {
           structure->fpr++;
         }
      }
      now=std::chrono::high_resolution_clock::now();
      structure->fpr/=double(g->newItems.size());
      microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
      microseconds/=1000;
      structure->queryTime+=microseconds;

    }
  }


  cout<<"Number of insertions = "<<countedKmers<<endl;
  cout<<"Number of unique items = "<<g->nunique_items<<endl;
  cout<<"Number of succesfull lookups = "<<num_queries<<endl;
  cout<<"Number of non succesfull lookups = "<<num_queries<<endl;
  cout<<"error margin in countmin = "<<(((double)countedKmers*e)/(countMinWidth))<<endl;

  cout<<"Name"<<"\t"<<"Size"<<"\t"<<"Loading Factor"<<"\t"<<"FPR"<<"\t"<<"Insertion Time"<<"\t"<<"QueryTime"<<endl;
  for(auto structure: dataStructures){
      structure->print();
  }
  //
  //   now = std::chrono::high_resolution_clock::now();
  //   if(dataStrucureInput=="bmqf")
  //   {
  //
  //   }
  //   else{
  //     for(int j=0;j<BufferSize;j++)
  //     {
  //       dataStructure->query(input[j]);
  //     }
  //   }
  //
  //   prev=now;
  //   now = std::chrono::high_resolution_clock::now();
  //
  //   microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  //   querytime=microseconds;
  //   {
  //     double millionInsertionSecond=(double)countedKmers/(double)insertiontime;
  //     cout<<capacity<<"\t"<<g->get_generated_elements()<<"\t"<<millionInsertionSecond;
  //     if (dataStrucureInput=="lmqf"){
  //       pair<int, int> tmp;
  //       layeredMQF* lmqf=((LMQF*)dataStructure)->get_MQF();
  //       tmp.first=qf_space(lmqf->firstLayer_singletons);
  //       tmp.second=qf_space(lmqf->secondLayer);
  //       int64_t tmp2=(int64_t)slotsUsedInCounting(lmqf->firstLayer_singletons);
  //       tmp2-=(int64_t)lmqf->firstLayer_singletons->metadata->noccupied_slots;
  //       cout<<"\t"<<tmp.first<<"\t"<<tmp.second<<"\t"<<tmp2;
  //     }
  //     cout<<endl;
  //     old_capacity=capacity;
  //
  //     capacity=dataStructure->space();
  //
  //     double millionQuerysSecond=(double)countedKmers/(double)querytime;
  //     result.push_back(make_pair(capacity,(double)millionQuerysSecond));
  //   }
  //
  //   capacity=dataStructure->space();
  //   if(dataStrucureInput=="mqf")
  //   {
  //     double slots_used=((MQF*)dataStructure)->calculate_slotsUsedInCounting();
  //     slots_used-=((MQF*)dataStructure)->get_MQF()->metadata->noccupied_slots;
  //     results_slotsUsed.push_back(make_pair(capacity,slots_used));
  //   }
  //
  //
  // }
  // cout<<"Capacity\t1M Query per second"<<endl;
  // for(int i=0;i<result.size();i++)
  // {
  //   cout<<result[i].first<<"\t"<<result[i].second<<endl;
  // }
  // if(dataStrucureInput=="mqf"){
  //   cout<<"Capacity\t%slots used in counting"<<endl;
  //   for(int i=0;i<results_slotsUsed.size();i++)
  //   {
  //     cout<<results_slotsUsed[i].first<<"\t"<<results_slotsUsed[i].second<<endl;
  //   }
  // }
  // cerr<<"#generated elements= "<<g->get_generated_elements()<<endl;
  return 0;
}
