#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include "utils.h"

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
