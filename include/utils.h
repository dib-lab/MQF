#ifndef _utils_H
#define _utils_H

#include <map>
#include <iostream>
#include <fstream>

namespace MQF {

    std::vector<std::string> split(const std::string &text, char sep);

    std::vector<int> key_to_vector_int(const std::string &key);

    void save_labels_map(std::map<uint64_t, std::vector<int> > *index, const char *fileName);

    std::map<uint64_t, std::vector<int> > *load_labels_map(const char *fileName);

    inline bool file_exists(const std::string &name) {
        std::ifstream f(name.c_str());
        return f.good();
    }
};
 #endif /* _utils_H */
