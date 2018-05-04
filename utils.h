#ifndef _utils_H
#define _utils_H

#include <map>
#include <iostream>
#include <fstream>

using namespace std;

std::vector<std::string> split(const std::string &text, char sep) ;

vector<int> key_to_vector_int(const string& key);

void save_inverted_index(std::map<uint64_t, std::vector<int> > * index,const char * fileName);

std::map<uint64_t, std::vector<int> > load_inverted_index(const char * fileName);
 #endif /* _utils_H */
