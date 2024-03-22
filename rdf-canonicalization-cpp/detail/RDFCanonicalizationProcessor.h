#ifndef RDFCANON_CPP_RDFCANONICALIZATIONPROCESSOR_H
#define RDFCANON_CPP_RDFCANONICALIZATIONPROCESSOR_H

#include <string>

#include <jsonld-cpp/jsoninc.h>

class JsonLdOptions;
namespace RDF {
    class RDFDataset;
}

struct RDFCanonicalizationProcessor {

    /**
     * Canonicalization Algorithm
     *
     * This algorithm converts an input RDFDataset into a canonicalized RDFDataset. This algorithm
     * will assign deterministic identifiers to any blank nodes in the input dataset.
     *
     * https://w3c-ccg.github.io/rdf-dataset-canonicalization/spec/#canonicalization-algorithm
     */
    static std::string canonicalize(const RDF::RDFDataset& dataset, const JsonLdOptions& options, nlohmann::json *issuedIdentifiersMap);

};


#endif //RDFCANON_CPP_RDFCANONICALIZATIONPROCESSOR_H
