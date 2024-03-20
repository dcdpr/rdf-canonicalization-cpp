#include "rdf-canonicalization-cpp/detail/Permutator.h"

#include <utility>
#include <algorithm>

using std::string;

Permutator::Permutator(std::vector<string> strings)
        : strings(std::move(strings)), done(false)
{
    std::sort(this->strings.begin(), this->strings.end());
}

bool Permutator::hasNext() const {
    return !done;
}

std::vector<string> Permutator::next() {
    std::vector<std::string> result;
    std::copy(strings.begin(), strings.end(),
              std::back_inserter(result));

    done = !std::next_permutation(strings.begin(), strings.end());

    return result;
}
