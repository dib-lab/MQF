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
using namespace std;

#include <iostream>




int main(int argc, char const *argv[]) {
  CLI::App app;
  string distribution="zipfian";
  double zipifian_coefficient=3.0;
  double uniform_coefficient=5;
  string dataStrucureInput="mqf";


  uint64_t qbits=20;
  uint64_t singleQbits=21;
  uint64_t slot_size=8;
  uint64_t fixedCounterSize=2;
  uint64_t num_elements=0;
  uint64_t grainSize=10000;

  app.add_option("-d,--distribution",distribution,
  "Distributions which items will be drew from. options are uniform,zipfian. Default is zipfian");
  app.add_option("-n,--num_elements",num_elements,
  "Number of elements to generate(0 means infinity). Default is 0");
  app.add_option("-z",zipifian_coefficient,"Zipifian Coeffiecient. Default =3")->group("Zipfian Distribution Options");
  app.add_option("-u",uniform_coefficient,
  "Frequency of items in the uinform distribution. Default =5")
  ->group("Uniform Distribution Options");
  app.add_option("-s,--datastructure",dataStrucureInput,
  "Datastructure to be benchmarked. Options are mqf, lmqf, cqf, and countmin. Default is mqf");

  app.add_option("-q,--qbits",qbits
  ,"Qbits used by quotient fiter. No slots equals to 2^qbits. default is 20")
  ->group("CQF/MQF options");

  app.add_option("-l,--qbits-singleton",singleQbits
  ,"first layer Qbits used by quotient fiter. No slots equals to 2^qbits. default is 21")
  ->group("Layered MQF options");

  app.add_option("-r,--slot-size",slot_size
  ,"Slot size used by quotient fiter. default is 8")
  ->group("CQF/MQF options");

  app.add_option("-f,--fixed-counter-size",fixedCounterSize
  ,"Fixed Counter Size used by MQF. default is 2")
  ->group("CQF/MQF options");

  app.add_option("-g,--grain-size",grainSize
  ,"Grain Size. default is 10,000");



  CLI11_PARSE(app, argc, (char**)argv);

  uint64_t BufferSize=grainSize;
    countingStructure* dataStructure;
    if(dataStrucureInput=="mqf"){
      dataStructure=new MQF(qbits,slot_size,fixedCounterSize);
    }
    else if(dataStrucureInput=="cqf")
    {
      dataStructure=new CQF(qbits,slot_size);
    }
    else if(dataStrucureInput=="countmin")
    {
      dataStructure=new countmin((1ULL<<(qbits-3)),8);
      //return -1;
    }
    else if(dataStrucureInput=="lmqf")
    {
      dataStructure=new LMQF(singleQbits,qbits,slot_size,fixedCounterSize);
    }
    else if(dataStrucureInput=="bmqf")
    {
      cout<<"Here"<<endl;
      dataStructure=new BMQF(singleQbits,qbits,slot_size,fixedCounterSize);
    }
    else{
      cout<<"Unknown datastructure"<<endl;
      return -1;
    }
  srand (1);




  generator *g;
  if(distribution=="uniform")
  {
    g=new uniformGenerator(num_elements,dataStructure->range(),uniform_coefficient);
  }
  else if(distribution=="zipfian"){
    g= new zipfGenerator(num_elements,dataStructure->range(),zipifian_coefficient);
  }
  else{
    cout<<"Unknown Distribution"<<endl;
    return -1;
  }





  uint64_t countedKmers=0;
  vector<uint64_t> input(BufferSize);
  auto now = std::chrono::high_resolution_clock::now();
  auto prev=now;
  now = std::chrono::high_resolution_clock::now();



  vector<pair<int,int> > lmqfSpace;
  vector<pair<double,double> > result;
  vector<pair<double,double> > results_slotsUsed;
  cout<<"Capacity\t1M insertions per second"<<endl;
  double capacity=dataStructure->space();


  uint64_t old_capacity=capacity;
  uint64_t insertiontime=0;
  uint64_t querytime=0;
  uint64_t curr_item;
  while(capacity<95&&g->hasMore())
  {
    for(int j=0;j<BufferSize;j++){
      if(!g->getElement(curr_item))
        break;
      input[j]=curr_item;
    }
    now = std::chrono::high_resolution_clock::now();
    for(int j=0;j<BufferSize;j++){
      dataStructure->insert(input[j],1);
      capacity=dataStructure->space();
      if(capacity>=95){
        break;
      }
    }
    auto prev=now;
    now = std::chrono::high_resolution_clock::now();
    auto microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
    microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
    insertiontime=microseconds;
    countedKmers=BufferSize;



    now = std::chrono::high_resolution_clock::now();
    if(dataStrucureInput=="bmqf")
    {
        ((BMQF*)dataStructure)->batchQuery(input);
    }
    else{
      for(int j=0;j<BufferSize;j++)
      {
        dataStructure->query(input[j]);
      }
    }

    prev=now;
    now = std::chrono::high_resolution_clock::now();

    microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
    querytime=microseconds;
    {
      double millionInsertionSecond=(double)countedKmers/(double)insertiontime;
      cout<<capacity<<"\t"<<g->get_generated_elements()<<"\t"<<millionInsertionSecond;
      if (dataStrucureInput=="lmqf"){
        pair<int, int> tmp;
        layeredMQF* lmqf=((LMQF*)dataStructure)->get_MQF();
        tmp.first=qf_space(lmqf->firstLayer_singletons);
        tmp.second=qf_space(lmqf->secondLayer);
        int64_t tmp2=(int64_t)slotsUsedInCounting(lmqf->firstLayer_singletons);
        tmp2-=(int64_t)lmqf->firstLayer_singletons->metadata->noccupied_slots;
        cout<<"\t"<<tmp.first<<"\t"<<tmp.second<<"\t"<<tmp2;
      }
      cout<<endl;
      old_capacity=capacity;

      capacity=dataStructure->space();

      double millionQuerysSecond=(double)countedKmers/(double)querytime;
      result.push_back(make_pair(capacity,(double)millionQuerysSecond));
    }

    capacity=dataStructure->space();
    if(dataStrucureInput=="mqf")
    {
      double slots_used=((MQF*)dataStructure)->calculate_slotsUsedInCounting();
      slots_used-=((MQF*)dataStructure)->get_MQF()->metadata->noccupied_slots;
      results_slotsUsed.push_back(make_pair(capacity,slots_used));
    }


  }
  cout<<"Capacity\t1M Query per second"<<endl;
  for(int i=0;i<result.size();i++)
  {
    cout<<result[i].first<<"\t"<<result[i].second<<endl;
  }
  if(dataStrucureInput=="mqf"){
    cout<<"Capacity\t%slots used in counting"<<endl;
    for(int i=0;i<results_slotsUsed.size();i++)
    {
      cout<<results_slotsUsed[i].first<<"\t"<<results_slotsUsed[i].second<<endl;
    }
  }
  cerr<<"#generated elements= "<<g->get_generated_elements()<<endl;
  return 0;
}
