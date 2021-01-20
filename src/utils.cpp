#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include "utils.h"
#include <limits>
#include "math.h"

using namespace std;
using namespace MQF;

std::vector<std::string> MQF::split(const std::string &text, char sep) {
  std::vector<std::string> tokens;
  std::size_t start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));
  return tokens;
}

vector<int> MQF::key_to_vector_int(const string& key){
  vector<string> tokens=MQF::split(key,';');
  vector<int> res;
  for(auto f:tokens)
  {
    res.push_back(atoi(f.c_str()));
  }
  return res;
}

void MQF::save_labels_map(std::map<uint64_t, std::vector<int> > * index,const char * fileName)
{
  ofstream out(fileName);
  auto it=index->begin();
  while( it != index->end() )
  {
    out<<it->first<<" ";
    for(auto i:it->second)
    {
      out<<i<<";";
    }
    out<<endl;
    it++;
  }
  out.close();
}


std::map<uint64_t, std::vector<int> >* MQF::load_labels_map(const char * fileName)
{
  std::map<uint64_t, std::vector<int> >* res=new std::map<uint64_t, std::vector<int> >();
  ifstream out(fileName);
  string key;
  uint64_t id;
  while(out>>id>>key){

    res->insert( make_pair(id,MQF::key_to_vector_int(key)) );
  }
  return res;
}


uint64_t MQF::estimateMemory(uint64_t nslots, uint64_t slotSize, uint64_t fcounter, uint64_t tagSize) {
  uint64_t SLOTS_PER_BLOCK_t = 64;
  uint64_t xnslots = nslots + 10 * sqrt((double) nslots);
  uint64_t nblocks = (xnslots + SLOTS_PER_BLOCK_t - 1) / SLOTS_PER_BLOCK_t;
  uint64_t blocksize = 17;

  return ((nblocks) * (blocksize + 8 * (slotSize + fcounter + tagSize))) / 1024;

}

bool MQF::isEnough(vector<uint64_t>& histogram, uint64_t noSlots, uint64_t fixedSizeCounter, uint64_t slotSize) {
  // cout<<"noSlots= "<<noSlots<<endl
  //     <<"fcounter= "<<fixedSizeCounter<<endl
  //     <<"slot size= "<<numHashBits<<endl;

  noSlots = (uint64_t) ((double) noSlots * 0.95);
  for (uint64_t i = 1; i < 1000; i++) {
    uint64_t usedSlots = 1;

    if (i > ((1ULL) << fixedSizeCounter) - 1) {
      uint64_t nSlots2 = 0;
      __uint128_t capacity;
      do {
        nSlots2++;
        capacity = ((__uint128_t) (1ULL) << (nSlots2 * slotSize + fixedSizeCounter)) - 1;
        //  cout<<"slots num "<<nSlots2<<" "<<capacity<<endl;
      } while ((__uint128_t) i > capacity);
      usedSlots += nSlots2;
    }
    //cout<<"i= "<<i<<"->"<<usedSlots<<" * "<<histogram[i]<<endl;
    if (noSlots >= (usedSlots * histogram[i])) {
      noSlots -= (usedSlots * histogram[i]);
    } else {
      //  cout<<"failed"<<endl<<endl;
      return false;
    }

  }
  //cout<<"success"<<endl<<endl;
  return true;
}
void MQF::estimateMemRequirement(vector<uint64_t>& histogram,
                            uint64_t numHashBits, uint64_t tagSize,
                            uint64_t *res_noSlots, uint64_t *res_fixedSizeCounter, uint64_t *res_memory) {
  uint64_t noDistinctKmers = 0, totalNumKmers=0;
  *res_memory = numeric_limits<uint64_t>::max();
  for (int i = 8; i < 64; i++) {
    uint64_t noSlots = (1ULL) << i;
    if (noSlots < noDistinctKmers)
      continue;
    bool moreWork = false;
    uint64_t slotSize = numHashBits - log2((double) noSlots);
    for (uint64_t fixedSizeCounter = 1; fixedSizeCounter < slotSize; fixedSizeCounter++) {
      if (isEnough(histogram, noSlots, fixedSizeCounter, slotSize)) {
        uint64_t tmpMem = estimateMemory(noSlots, slotSize, fixedSizeCounter, tagSize);
        if (*res_memory > tmpMem) {
          *res_memory = tmpMem;
          *res_fixedSizeCounter = fixedSizeCounter;
          *res_noSlots = i;
          moreWork = true;
        } else {
          break;
        }
      }

    }
    if (!moreWork && *res_memory != numeric_limits<uint64_t>::max())
      break;
  }
  if (*res_memory == numeric_limits<uint64_t>::max()) {
    throw std::overflow_error(
            "Data limits exceeds MQF capabilities(> uint64). Check if ntcard file is corrupted");
  }


}
