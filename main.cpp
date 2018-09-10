#include "gqf.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include<iostream>
#include <fstream>
#include "hashutil.h"
#include "kmer.h"
using namespace std;

int main(int argc, char const *argv[]) {

  uint64_t qbits=atoi(argv[1]);
  uint64_t fixed_counter=atoi(argv[2]);
  ifstream dataset(argv[3]);
  ifstream test_dataset(argv[4]);
  QF qf;
  srand (1);

  uint64_t num_hash_bits=qbits+8;

  qf_init(&qf, (1ULL<<qbits), num_hash_bits, 0,fixed_counter, true, "", 2038074761);
  string kmer;
  uint64_t countedKmers=0;
  while(dataset>>kmer)
  {
    uint64_t item=kmercounting::str_to_int(kmer);
    uint64_t hash=kmercounting::HashUtil::MurmurHash64A(((void*)&item), sizeof(item),qf.metadata->seed);
    hash=hash%qf.metadata->range;
    qf_insert(&qf,hash,1,false,false);

  }
  uint64_t count;
  while(test_dataset>>kmer>>count)
  {
    uint64_t item=kmercounting::str_to_int(kmer);
    uint64_t hash=kmercounting::HashUtil::MurmurHash64A(((void*)&item), sizeof(item),qf.metadata->seed);
    hash=hash%qf.metadata->range;
    uint64_t tmp_count=qf_count_key(&qf, hash);
    if(tmp_count<count){
      cerr<<"Error in counting "<<count<<" -> "<<tmp_count<<endl;
    }
  }
  cout<<qf.metadata->size<<endl
  ;
  return 0;
}
