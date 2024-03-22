#ifndef RDFCANON_CPP_CANONICALIZATIONPROCESSOR_H
#define RDFCANON_CPP_CANONICALIZATIONPROCESSOR_H

#include <string>

#include <jsonld-cpp/jsoninc.h>

class JsonLdOptions;


namespace CanonicalizationProcessor {

    // Canonicalize JSON-LD or RDF document located at filename or IRI into
    // an RdfDataset according to the steps in the RDF Canonicalization Algorithm
    // See: https://w3c-ccg.github.io/rdf-dataset-canonicalization/spec/#canonicalization-algorithm
    std::string canonicalize(const std::string& documentLocation, JsonLdOptions& options);

    // Canonicalize JSON-LD or RDF document located at filename or IRI into
    // an RdfDataset according to the steps in the RDF Canonicalization Algorithm
    // See: https://w3c-ccg.github.io/rdf-dataset-canonicalization/spec/#canonicalization-algorithm
    //
    // This function optionally returns the issuedIdentifiersMap.
    std::string canonicalize(const std::string& documentLocation, JsonLdOptions& options, nlohmann::json *issuedIdentifiersMap);
}

#endif //RDFCANON_CPP_CANONICALIZATIONPROCESSOR_H
