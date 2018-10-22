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


  uint64_t qbits=25;

  uint64_t kSize=25;
  uint64_t num_elements=0;
  string fqPath;
  uint64_t fixedCounterSize=2;

  app.add_option("-d,--distribution",distribution,
  "Distributions which items will be drew from. options are uniform,kmers,zipfian. Default is zipfian");
  // app.add_option("-n,--num_elements",num_elements,
  // "Number of elements to generate(0 means infinity). Default is 0");
  app.add_option("-z",zipifian_coefficient,"Zipifian Coeffiecient. Default =3")->group("Zipfian Distribution Options");
  app.add_option("-u",uniform_coefficient,
  "Frequency of items in the uinform distribution. Default =5")
  ->group("Uniform Distribution Options");
  // app.add_option("-s,--datastructure",dataStrucureInput,
  // "Datastructure to be benchmarked. Options are mqf, lmqf, cqf, and countmin. Default is mqf");

  app.add_option("-s,--size",qbits
  ,"No slots equals to 2^(s). default is 25")
  ->group("Counting Strucutre Options");

  app.add_option("-k",fqPath,"Fastq file to draw the kmers");

  app.add_option("-f,--fixed-counter-size",fixedCounterSize
  ,"Fixed Counter Size used by MQF. default is 2")
  ->group("CQF/MQF options");








  CLI11_PARSE(app, argc, (char**)argv);
  uint64_t slot_size=39-qbits;
  if(distribution=="kmers")
  {
    slot_size=kSize*2-qbits;
  }

  cout<<"Command = ";
  for(int i=0;i<argc;i++)
  {
    cout<<argv[i]<<" ";
  }
  cout<<endl;


  num_elements=(1ULL<<qbits)*5;
  cout<<"Number of elements = "<<num_elements<<endl;
  uint64_t BufferSize=num_elements/10;
  uint64_t num_queries=num_elements/8;

  vector<countingStructure*> dataStructures;
  double p=(double)(slot_size+qbits);
  dataStructures.push_back(new MQF(qbits,slot_size,fixedCounterSize));
  dataStructures.push_back(new CQF(qbits,slot_size));


  cout<<"CQF/MQFF Q = "<<qbits<<endl;
  cout<<"CQF/MQF slot size= "<<slot_size<<endl;
  cout<<"CQF/MQF num hashbits= "<<p<<endl;
  cout<<"MQF Size= "<<dataStructures[0]->size<<endl;
  cout<<"CQF Size= "<<dataStructures[1]->size<<endl;

  //nt countMinDepth=(p*log(2.0)-log((double)num_elements))+1;
  //uint64_t countMinWidth = dataStructures[0]->size / countMinDepth;
  //double e=2.71828;
//  uint64_t countMinWidth=(e*99824410)+1;
  //cout<<"Count Min Sketch Width= "<<countMinWidth<<endl;
  //cout<<"Count Min Sketch Depth= "<<countMinDepth<<endl;
  uint64_t range=(1ULL<<(int)p);

//  dataStructures.push_back(new countmin(countMinWidth,countMinDepth));
//  dataStructures.push_back(new countminKhmer(countMinWidth,countMinDepth));
  //dataStructure.push_back(new LMQF(singleQbits,qbits,slot_size,fixedCounterSize));
  //dataStructures.push_back(new BMQF(qbits,qbits-2,slot_size,fixedCounterSize));
//  cout<<"Buffered MQF buffer Q = "<<qbits-2<<endl;
//  cout<<"STXXL buffer size= "<<((BMQF*)dataStructures.back())->bsize<<"MB"<<endl;


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
  cout<<"Distribution = "<<g->name<<endl;
  cout<<g->parameters<<endl;



  //
  //
  uint64_t countedKmers=0;
  vector<uint64_t> insertions(BufferSize);


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

  uint64_t curr_item;
  uint64_t moreWork=true;

  vector<uint64_t> steps;
// check the size of cqf
   while(moreWork&& g->hasMore())
   {
     for(int j=0;j<BufferSize;j++){
       if(!g->getElement(curr_item)){
         break;
       }
       insertions[j]=curr_item;
     }
     cerr<<"Number of unique items generated so far ="<<g->nunique_items<<endl;
     countedKmers+=BufferSize;
     steps.push_back(countedKmers);
     moreWork=false;
     for(auto structure: dataStructures){


       cerr<<"Inserting "<<BufferSize<<" to "<<structure->name<<"("<<structure->space()<<"%)"<<endl;
       int j=0;
       for(;j<BufferSize;j++){
         if(structure->space()>90)
            break;
          structure->insert(insertions[j],1);
        }
        if(structure->space()<90)
          moreWork=true;
        if(j==BufferSize){
          structure->loadingFactors.push_back(structure->space());
        }
        else{
          structure->loadingFactors.push_back(0);
        }
     }

  }


  cout<<"Number of insertions = "<<countedKmers<<endl;

  cout<<"No Elements";
  for(auto structure: dataStructures){
    cout<<"\t"<<structure->name;
  }
  cout<<endl;
  for(int i=0;i<steps.size();i++)
  {
    cout<<steps[i];
    for(auto structure: dataStructures){
      cout<<"\t";
      if(i<structure->loadingFactors.size())
      {
        cout<<structure->loadingFactors[i];
      }
      else{
        cout<<"\t";
      }
    }
    cout<<endl;
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
