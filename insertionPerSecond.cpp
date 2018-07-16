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
  QF qf;
  srand (1);

  uint64_t num_hash_bits=qbits+8;

  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 0,1, true, "", 2038074761);
  string kmer;
  uint64_t count;
  uint64_t countedKmers=0;
  vector<uint64_t> input;
  uint64_t hash;
  auto now = std::chrono::high_resolution_clock::now();
  while(dataset>>kmer)
  {
    uint64_t item=kmercounting::str_to_int(kmer);
    hash=kmercounting::HashUtil::MurmurHash64A(((void*)&item), sizeof(item),qf.metadata->seed);
    hash=hash%qf.metadata->range;
    input.push_back(hash);
  }
  auto prev=now;
  now = std::chrono::high_resolution_clock::now();
  auto microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
  cout<<"Loading finished in "<<microseconds<<endl;
  vector<pair<double,double> > result;

  for(int i=0;i<input.size();i++)
  {

    if(countedKmers %1000000==0){
      auto prev=now;
      now = std::chrono::high_resolution_clock::now();
      auto microseconds = (chrono::duration_cast<chrono::microseconds>(now-prev)).count();
      if(countedKmers!=0){
        double millionInsertionSecond=1000000.0/(microseconds);
        double capacity=qf_space(&qf);
        result.push_back(make_pair(capacity,(double)microseconds));
      }
      countedKmers=0;
      now = std::chrono::high_resolution_clock::now();
    }
    qf_insert(&qf,input[i],1,false,false);
    countedKmers++;
  }
  cout<<"Capacity\tInsertion time for 1M Kmer"<<endl;
  for(int i=0;i<result.size();i++)
  {
    cout<<result[i].first<<"\t"<<result[i].second<<endl;
  }
  qf_destroy(&qf);
  return 0;
}
