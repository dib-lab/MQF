#ifndef _utils_H
#define _utils_H

#include <map>
#include <iostream>
#include <fstream>
#include <vector>

namespace MQF {

    std::vector<std::string> split(const std::string &text, char sep);

    std::vector<int> key_to_vector_int(const std::string &key);

    void save_labels_map(std::map<uint64_t, std::vector<int> > *index, const char *fileName);

    std::map<uint64_t, std::vector<int> > *load_labels_map(const char *fileName);

    inline bool file_exists(const std::string &name) {
        std::ifstream f(name.c_str());
        return f.good();
    }

    uint64_t estimateMemory(uint64_t nslots, uint64_t slotSize, uint64_t fcounter, uint64_t tagSize) ;
    bool isEnough(std::vector<uint64_t>& histogram, uint64_t noSlots, uint64_t fixedSizeCounter, uint64_t slotSize) ;
    void estimateMemRequirement(std::vector<uint64_t>& histogram,
                                uint64_t numHashBits, uint64_t tagSize,
                                uint64_t *res_noSlots, uint64_t *res_fixedSizeCounter, uint64_t *res_memory) ;

};
 #endif /* _utils_H */
