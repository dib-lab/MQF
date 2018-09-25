#include "gqf.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include <fstream>
#include "hashutil.h"
#include "kmer.h"
#include <vector>
#include <chrono>
using namespace std;

int main(int argc, char const *argv[]) {

  ifstream dataset(argv[1]);
  uint64_t qbits=atoi(argv[2]);
  cqf_qf_ cqf_qf_;
  srand (1);

  uint64_t num_hash_bits=qbits+8;

  cqf_qf__init(&cqf_qf_, (1ULL<<qbits), num_hash_bits, 0, true, "", 2038074761);

  string kmer;
  uint64_t count;
  uint64_t countedKmers=0;
  vector<uint64_t> input;
  uint64_t hash;
  auto now = std::chrono::high_resolution_clock::now();
  while(dataset>>hash)
  {

    hash=hash%cqf_qf_.metadata->range;
  }
  auto prev=now;
  now = std::chrono::high_resolution_clock::now();
  auto microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  //cout<<"Loading finished in "<<microseconds<<endl;
  vector<pair<double,double> > result;
  size_t query_index=0;
  cout<<"Capacity\t1M insertions per second"<<endl;
  while(dataset>>hash)
  {

    if(countedKmers %1000000==0){
      auto prev=now;
      now = std::chrono::high_resolution_clock::now();
      auto microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
      if(countedKmers!=0){
        double millionInsertionSecond=1000000.0/(microseconds);
        double capacity=cqf_qf__space(&cqf_qf_);
        //double capacity=0;
        //result.push_back(make_pair(capacity,(double)millionInsertionSecond));
        cout<<capacity<<"\t"<<millionInsertionSecond<<endl;
        now = std::chrono::high_resolution_clock::now();
        for(int j=0;j<1000000;j++)
        {
          cqf_qf__count_key_value(&cqf_qf_,input[query_index],0);
          query_index=(query_index+1)%input.size();
        }
        now = std::chrono::high_resolution_clock::now();
        microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
        double millionQuerysSecond=1000000.0/(microseconds);
        result.push_back(make_pair(capacity,(double)millionQuerysSecond));
        if(capacity>=95){
          break;
        }
      }
      countedKmers=0;
      now = std::chrono::high_resolution_clock::now();
    }
    hash=hash%cqf_qf_.metadata->range;
    cqf_qf__insert(&cqf_qf_,hash,0,1,false,false);
    countedKmers++;
  }

  cout<<"Capacity\t1M Querys per second"<<endl;
  for(int i=0;i<result.size();i++)
  {
    cout<<result[i].first<<"\t"<<result[i].second<<endl;
  }



  cqf_qf_destroy(&cqf_qf_,true);
  return 0;
}
