#include "rdf-canonicalization-cpp/CanonicalizationProcessor.h"

#include <sstream>

#include <jsonld-cpp/MediaType.h>
#include <jsonld-cpp/JsonLdOptions.h>
#include <jsonld-cpp/RemoteDocument.h>
#include <jsonld-cpp/RDFDataset.h>
#include <jsonld-cpp/JsonLdError.h>
#include <jsonld-cpp/JsonLdProcessor.h>

#include "rdf-canonicalization-cpp/detail/RDFCanonicalizationProcessor.h"

using RDF::RDFDataset;


std::string CanonicalizationProcessor::canonicalize(const std::string& documentLocation, JsonLdOptions& options, nlohmann::json *issuedIdentifiersMap) {

    auto document = options.getDocumentLoader()->loadDocument(documentLocation);

    if(document->getContentType() == MediaType::json_ld()) {
        RDFDataset dataset = JsonLdProcessor::toRDF(documentLocation, options);
        return RDFCanonicalizationProcessor::canonicalize(dataset, options, issuedIdentifiersMap);
    }
    else if(document->getContentType() == MediaType::n_quads()) {
        RDFDataset dataset = document->getRDFContent();
        return RDFCanonicalizationProcessor::canonicalize(dataset, options, issuedIdentifiersMap);
    }
    else {
        std::stringstream ss;
        ss << "Unsupported content type: '" << document->getContentType()
           << "'. Supported content types for normalization are: "
           << MediaType::json_ld() << " and " << MediaType::n_quads();
        throw JsonLdError(JsonLdError::LoadingDocumentFailed, ss.str());
    }

}

std::string CanonicalizationProcessor::canonicalize(const std::string& documentLocation, JsonLdOptions& options) {
    return canonicalize(documentLocation, options, nullptr);
}


