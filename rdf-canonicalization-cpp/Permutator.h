#ifndef RDFCANON_CPP_PERMUTATOR_H
#define RDFCANON_CPP_PERMUTATOR_H

#include <vector>
#include <map>
#include <string>


class Permutator {
private:
    std::vector<std::string> strings;
    bool done;

public:
    explicit Permutator(std::vector <std::string> strings);
    bool hasNext() const;
    std::vector<std::string> next();
};

#endif //RDFCANON_CPP_PERMUTATOR_H
