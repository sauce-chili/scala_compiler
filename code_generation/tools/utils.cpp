#include "utils.h"
#include "../semantic/tools/tools.h"

void merge(std::vector<char> &bytes, const std::vector<char> &buffer){
    bytes.insert(bytes.end(), all(buffer));
}

void merge(std::vector<bool> &bytes, const std::vector<bool> &buffer){
    bytes.insert(bytes.end(), all(buffer));
}
