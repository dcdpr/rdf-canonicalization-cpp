#include <iostream>
#include "rdf-canonicalization-cpp/RDFCanonicalizationProcessor.h"
#include "rdf-canonicalization-cpp/Permutator.h"
#include <jsonld-cpp/JsonLdProcessor.h>
#include <jsonld-cpp/JsonLdError.h>
#include <jsonld-cpp/NQuadsSerialization.h>
#include <jsonld-cpp/RemoteDocument.h>
#include <jsonld-cpp/RDFQuad.h>
#include <jsonld-cpp/RDFTriple.h>
#include <jsonld-cpp/RDFDataset.h>
#include <jsonld-cpp/sha256.h>
#include <jsonld-cpp/sha384.h>
#include <jsonld-cpp/BlankNodeNames.h>

namespace {

    struct CanonicalizationState {

        // blank node identifier to quads map
        std::map<std::string, std::vector<RDF::RDFQuad>> bnode_to_quads;

        // hash to blank node identifier map
        std::map<std::string, std::vector<std::string>> hash_to_bnodes;

        // canonical issuer
        BlankNodeNames canonical_issuer{"_:c14n"};

        // keep track of insertion order of keys for bnode_to_quads
        std::vector<std::string> bnode_to_quads_insertion_order_keys;

        // keep track of how many n-degree iterations we are doing
        int maxIterations = 500;
        int currentIteration = 0;

        // which hash function to use
        std::string (*hash)(const std::string & input){};
        std::string (*hashVec)(const std::vector<std::string> & input){};

        void addToQuadsMapIfBlankNode(const std::shared_ptr<RDF::Node>& node, const RDF::RDFQuad& quad) {
            if(node != nullptr) {
                if(node->isBlankNode()) {
                    std::string id = node->getValue();
                    if(!bnode_to_quads.count(id)) {
                        std::vector<RDF::RDFQuad> tmp;
                        bnode_to_quads[id] = tmp;
                        bnode_to_quads_insertion_order_keys.push_back(id);
                    }
                    bnode_to_quads[id].push_back(quad);
                }
            }
        }

        void addToBlankNodeMap(const std::string& hashOfBlankNodeId, const std::string& blankNodeId) {
            if(!hash_to_bnodes.count(hashOfBlankNodeId)) {
                std::vector<std::string> tmp;
                hash_to_bnodes[hashOfBlankNodeId] = tmp;
            }
            hash_to_bnodes[hashOfBlankNodeId].push_back(blankNodeId);
        }

        void replaceNonCanonicalIdentifiers(const std::shared_ptr<RDF::Node>& node) {
            if(node != nullptr) {
                if(node->isBlankNode()) {
                    std::string id = node->getValue();
                    if(id.find("_:c14n") != 0) {
                        node->setValue(canonical_issuer.get(id));
                    }
                }
            }
        }
    };

    struct HashNDegreeQuadsResult {
        std::string hash;
        BlankNodeNames issuerUsed;
    };

    /**
     * Converts an RDFQuad into a string in the form of an N-Quad
     *
     * This function is slightly modified from jsonld-cpp's NQuadsSerialization::toNQuad() in
     * order to conform to the RDF Canonicalization spec. See the description of the Hash First
     * Degree Quads algorithm:
     * https://w3c-ccg.github.io/rdf-dataset-canonicalization/spec/#hash-first-degree-quads
     */
    std::string toNQuadForNormalization(const RDF::RDFQuad& quad, const std::string& referenceBlankNode) {
        std::stringstream ss;

        // subject: IRI or blank node (https://www.w3.org/TR/rdf11-concepts/#section-triples)
        std::shared_ptr<RDF::Node> s = quad.getSubject();
        if (s->isIRI())
            NQuadsSerialization::outputIRI(s->getValue(), ss);
        else if (s->isBlankNode())
            ss << ((s->getValue() == referenceBlankNode) ? "_:a" : "_:z");
        else
            throw JsonLdError(JsonLdError::IllegalArgument,
                              "toNQuadForNormalization: subject must be an IRI or a blank node");

        ss << " ";

        // predicate: only IRI (https://www.w3.org/TR/rdf11-concepts/#section-triples)
        std::shared_ptr<RDF::Node> p = quad.getPredicate();
        if (p->isIRI())
            NQuadsSerialization::outputIRI(p->getValue(), ss);
        else
            throw JsonLdError(JsonLdError::IllegalArgument,
                              "toNQuadForNormalization: predicate must be an IRI");

        ss << " ";

        // object: IRI, blank node or literal (https://www.w3.org/TR/rdf11-concepts/#section-triples)
        std::shared_ptr<RDF::Node> o = quad.getObject();
        if (o->isIRI())
            NQuadsSerialization::outputIRI(o->getValue(), ss);
        else if (o->isBlankNode())
            ss << ((o->getValue() == referenceBlankNode) ? "_:a" : "_:z");
        else {
            ss << "\"";
            ss << NQuadsSerialization::escape(o->getValue());
            ss << "\"";
            if (o->getDatatype() == JsonLdConsts::RDF_LANGSTRING) {
                ss << "@" << o->getLanguage();
            } else if (o->getDatatype() != JsonLdConsts::XSD_STRING) {
                ss << "^^<";
                ss << NQuadsSerialization::escape(o->getDatatype());
                ss << ">";
            }
        }

        ss << " ";

        // graph: IRI or blank node (https://www.w3.org/TR/rdf11-concepts/#section-triples)
        std::shared_ptr<RDF::Node> g = quad.getGraph();
        if (g != nullptr && g->getValue() != JsonLdConsts::DEFAULT) {
            if (g->isIRI())
                NQuadsSerialization::outputIRI(g->getValue(), ss);
            else if (g->isBlankNode())
                ss << ((g->getValue() == referenceBlankNode) ? "_:a" : "_:z");
            else
                throw JsonLdError(JsonLdError::IllegalArgument,
                                  "toNQuadForNormalization: graph must be an IRI or blank node");

            ss << " ";
        }

        ss << ".\n";

        return ss.str();
    }


    std::string
    hashFirstDegreeQuads(CanonicalizationState & state, const std::string& referenceBlankNodeId) {

        // Comments in this function are labeled with numbers that correspond to sections
        // from the description of the Hash First Degree Quads algorithm.
        // https://w3c-ccg.github.io/rdf-dataset-canonicalization/spec/#hash-first-degree-quads

        // 1)
        // Initialize nquads to an empty list. It will be used to store quads in N-Quads format.
        std::vector<std::string> nquads;

        // 2)
        // Get the list of quads quads associated with the reference blank node identifier in
        // the blank node to quads map.
        std::vector<RDF::RDFQuad> quads = state.bnode_to_quads.at(referenceBlankNodeId);
        nquads.reserve(quads.size());

        // 3)
        // For each quad quad in quads:
        for (const auto & quad : quads) {
            // 3.1)
            // Serialize the quad in N-Quads format with the following special rule:
            // 3.1.1)
            // If any component in quad is an blank node, then serialize it using a special
            // identifier as follows:
            // 3.1.1.1)
            // If the blank node's existing blank node identifier matches the reference blank
            // node identifier then use the blank node identifier _:a, otherwise, use the blank
            // node identifier _:z.
            nquads.push_back(toNQuadForNormalization(quad, referenceBlankNodeId));
        }

        // 4)
        // Sort nquads in lexicographical order.
        std::sort(nquads.begin(), nquads.end());

        // 5)
        // Return the hash that results from passing the sorted, joined nquads through the hash algorithm.
        std::string hash = state.hashVec(nquads);

        return hash;
    }

    std::string
    hashRelatedBlankNode(CanonicalizationState & state, const std::string& related, const RDF::RDFQuad& quad,
                         BlankNodeNames & issuer, const std::string& position) {

        // Comments in this function are labeled with numbers that correspond to sections
        // from the description of the Hash Related Blank Node algorithm.
        // https://w3c-ccg.github.io/rdf-dataset-canonicalization/spec/#hash-related-blank-node

        // 1)
        // Set the identifier to use for related, preferring first the canonical identifier for
        // related if issued, second the identifier issued by issuer if issued, and last, if
        // necessary, the result of the Hash First Degree Quads algorithm, passing related.
        std::string identifier;
        if(state.canonical_issuer.exists(related))
            identifier = state.canonical_issuer.get(related);
        else if(issuer.exists(related))
            identifier = issuer.get(related);
        else
            identifier = hashFirstDegreeQuads(state, related);

        // 2)
        // Initialize a string input to the value of position.
        std::string input = position;

        // 3)
        // If position is not g, append <, the value of the predicate in quad, and > to input.
        if(position != "g")
            input += "<" + quad.getPredicate()->getValue() + ">";

        // 4)
        // Append identifier to input.
        input += identifier;

        // 5)
        // Return the hash that results from passing input through the hash algorithm.
        return state.hash(input);
    }

    HashNDegreeQuadsResult
    hashNDegreeQuads(CanonicalizationState & state, const std::string& identifier, BlankNodeNames & issuer) {

        if(state.currentIteration > state.maxIterations) {
            throw JsonLdError("too many iterations");
        }
        state.currentIteration++;

        // Comments in this function are labeled with numbers that correspond to sections
        // from the description of the Hash N Degree Quads algorithm.
        // https://w3c-ccg.github.io/rdf-dataset-canonicalization/spec/#hash-n-degree-quads

        // 1)
        // Create a hash to related blank nodes map for storing hashes that identify related blank nodes.
        std::map<std::string, std::vector<std::string>> hash_to_related_bnodes;

        // 2)
        // Get a reference, quads, to the list of quads in the blank node to quads map for the key identifier.
        auto & quads = state.bnode_to_quads[identifier];

        // 3)
        // For each quad in quads:
        for(const auto& quad: quads) {
            // 3.1)
            // For each component in quad, where component is the subject, object, or graph name, and
            // it is a blank node that is not identified by identifier:
            std::map<std::string, std::shared_ptr<RDF::Node>> components{
                    {"s",quad.getSubject()}, {"o",quad.getObject()}, {"g",quad.getGraph()}};
            for(const auto& entry : components) {
                std::string position = entry.first;
                std::shared_ptr<RDF::Node> component = entry.second;
                if(!component || !component->isBlankNode() || component->getValue() == identifier)
                    continue;

                // 3.1.1)
                // Set hash to the result of the Hash Related Blank Node algorithm, passing the
                // blank node identifier for component as related, quad, path identifier issuer
                // as issuer, and position as either s, o, or g based on whether component is a
                // subject, object, graph name, respectively.
                std::string hash = hashRelatedBlankNode(state, component->getValue(), quad, issuer, position);

                // 3.1.2)
                // Add a mapping of hash to the blank node identifier for component to hash to
                // related blank nodes map, adding an entry as necessary.
                if(!hash_to_related_bnodes.count(hash)) {
                    std::vector<std::string> tmp;
                    hash_to_related_bnodes[hash] = tmp;
                }
                hash_to_related_bnodes[hash].push_back(component->getValue());
            }
        }

        // 4)
        // Create an empty string, data to hash.
        std::string dataToHash;

        // 5)
        // For each related hash to blank node list mapping in hash to related blank nodes
        // map, sorted lexicographically by related hash:
        std::vector<std::string> hashes;
        hashes.reserve(hash_to_related_bnodes.size());
        for (auto & hash_to_bnode : hash_to_related_bnodes) {
            hashes.push_back(hash_to_bnode.first);
        }
        for(const auto& hash : hashes) {
            // 5.1)
            // Append the related hash to the data to hash.
            dataToHash += hash;

            // 5.2)
            // Create a string chosen path.
            std::string chosenPath;

            // 5.3)
            // Create an unset chosen issuer variable.
            BlankNodeNames chosenIssuer;

            // 5.4)
            // For each permutation of blank node list:
            std::vector<std::string> blankNodeList = hash_to_related_bnodes[hash];
            Permutator bnodePermutations(blankNodeList);
            while(bnodePermutations.hasNext()) {
                std::vector<std::string> permutation = bnodePermutations.next();
                bool skipToNextPermutation = false;

                // 5.4.1)
                // Create a copy of issuer, issuer copy.
                BlankNodeNames issuerCopy = issuer;

                // 5.4.2)
                // Create a string path.
                std::string path;

                // 5.4.3)
                // Create a recursion list, to store blank node identifiers that must be recursively
                // processed by this algorithm.
                std::vector<std::string> recursionList;

                // 5.4.4)
                // For each related in permutation:
                for(const auto& related : permutation) {

                    // 5.4.4.1)
                    // If a canonical identifier has been issued for related, append it to path.
                    if(state.canonical_issuer.exists(related))
                        path += state.canonical_issuer.get(related);

                    // 5.4.4.2)
                    // Otherwise:
                    else {
                        // 5.4.4.2.1)
                        // If issuer copy has not issued an identifier for related, append related
                        // to recursion list.
                        if(!issuerCopy.exists(related))
                            recursionList.push_back(related);

                        // 5.4.4.2.2)
                        // Use the Issue Identifier algorithm, passing issuer copy and related and
                        // append the result to path.
                        path += issuerCopy.get(related);
                    }
                    // 5.4.4.3)
                    // If chosen path is not empty and the length of path is greater than or equal
                    // to the length of chosen path and path is lexicographically greater than chosen
                    // path, then skip to the next permutation.
                    if(!chosenPath.empty() &&
                       path.size() >= chosenPath.size() &&
                       path > chosenPath) {
                        skipToNextPermutation = true;
                        break;
                    }
                }

                if(skipToNextPermutation)
                    continue;

                // 5.4.5)
                // For each related in recursion list:
                for(const auto& related : recursionList) {
                    // 5.4.5.1)
                    // Set result to the result of recursively executing the Hash N-Degree Quads
                    // algorithm, passing related for identifier and issuer copy for path identifier
                    // issuer.
                    HashNDegreeQuadsResult result = hashNDegreeQuads(state, related, issuerCopy);

                    // 5.4.5.2)
                    // Use the Issue Identifier algorithm, passing issuer copy and related and
                    // append the result to path.
                    path += issuerCopy.get(related);

                    // 5.4.5.3)
                    // Append <, the hash in result, and > to path.
                    path += "<" + result.hash + ">";

                    // 5.4.5.4)
                    // Set issuer copy to the identifier issuer in result.
                    // note: we don't do this since we passed a reference

                    // 5.4.5.5)
                    // If chosen path is not empty and the length of path is greater than or
                    // equal to the length of chosen path and path is lexicographically greater
                    // than chosen path, then skip to the next permutation.
                    if(!chosenPath.empty() &&
                       path.size() >= chosenPath.size() &&
                       path > chosenPath) {
                        skipToNextPermutation = true;
                        break;
                    }
                }

                if(skipToNextPermutation)
                    continue;

                // 5.4.6)
                // If chosen path is empty or path is lexicographically less than chosen path, set
                // chosen path to path and chosen issuer to issuer copy.
                if(chosenPath.empty() || path < chosenPath) {
                    chosenPath = path;
                    chosenIssuer = issuerCopy;
                }
            }

            // 5.5)
            // Append chosen path to data to hash.
            dataToHash += chosenPath;

            // 5.6)
            // Replace issuer, by reference, with chosen issuer.
            issuer = chosenIssuer;

        }

        // 6)
        // Return issuer and the hash that results from passing data to hash through the hash algorithm.
        HashNDegreeQuadsResult result;
        result.hash = state.hash(dataToHash);
        result.issuerUsed = issuer;

        return result;
    }


    RDF::RDFDataset canonicalizeDataset(const RDF::RDFDataset &dataset, const JsonLdOptions& options, nlohmann::json *issuedIdentifiersMap) {

        // Comments in this function are labeled with numbers that correspond to sections
        // from the description of the RDF dataset canonicalization algorithm.
        // https://w3c-ccg.github.io/rdf-dataset-canonicalization/spec/#canonicalization-algorithm

        // 1)
        // Create the canonicalization state.
        CanonicalizationState state;

        // set function pointers for which hash algorithm to use
        if(options.getHashAlgorithm() == "SHA384") {
            state.hash = sha384;
            state.hashVec = sha384;
        }
        else {
            state.hash = sha256;
            state.hashVec = sha256;
        }

        // 2)
        // For every quad in input dataset:
        // todo: make a better worded note that the reason we want to get quads from the
        // dataset rather than graphnames -> triples, is that when we later
        // need to hash each data item, it requires that we have the graphname
        // represented in the data structure so its bytes can be included in the line's hash.
        // note2: should add this to the rdfquad definition
        std::vector<RDF::RDFQuad> quads = dataset.getAllGraphsAsQuads();
        for(const auto& quad : quads) {
            // 2.1)
            // For each blank node that occurs in the quad, add a reference to the quad using
            // the blank node identifier in the blank node to quads map, creating a new entry
            // if necessary.

            // note: any part of the quad can be a blank node in JSON-LD, but RDF doesn't support
            // a predicate as a blank node, so we ignore those.

            // todo: what the heck?
            // note: (from spec) It seems that quads must be normalized, so that literals with different
            // syntactic representations but the same semantic representations are merged, and that two
            // graphs differing in the syntactic representation of a literal will produce the same set
            // of blank node identifiers.

            state.addToQuadsMapIfBlankNode(quad.getSubject(), quad);
            state.addToQuadsMapIfBlankNode(quad.getObject(), quad);
            state.addToQuadsMapIfBlankNode(quad.getGraph(), quad);
        }

        // 3)
        // Create a list of non-normalized blank node identifiers non-normalized identifiers
        // and populate it using the keys from the blank node to quads map.
        std::vector<std::string> nonNormalizedIdentifiers = state.bnode_to_quads_insertion_order_keys;

        // 4)
        // Initialize simple, a boolean flag, to true.
        bool simple = true;

        // 5)
        // While simple is true, issue canonical identifiers for blank nodes:
        while(simple) {

            // 5.1)
            // Set simple to false.
            simple = false;

            // 5.2)
            // Clear hash to blank nodes map.
            state.hash_to_bnodes.clear();

            // 5.3)
            // For each blank node identifier identifier in non-normalized identifiers:
            for(const auto& identifier : nonNormalizedIdentifiers) {

                // 5.3.1)
                // Create a hash, hash, according to the Hash First Degree Quads algorithm.
                std::string hash = hashFirstDegreeQuads(state, identifier);

                // 5.3.2)
                // Add hash and identifier to hash to blank nodes map, creating a new entry if necessary.
                state.addToBlankNodeMap(hash, identifier);
            }

            // 5.4)
            // For each hash to identifier list mapping in hash to blank nodes map,
            // lexicographically-sorted by hash:
            std::vector<std::string> hashes;
            hashes.reserve(state.hash_to_bnodes.size());
            for (auto & hash_to_bnode : state.hash_to_bnodes) {
                hashes.push_back(hash_to_bnode.first);
            }
            for(const auto& hash : hashes) {
                std::vector<std::string> identifierList = state.hash_to_bnodes[hash];

                // 5.4.1)
                // If the length of identifier list is greater than 1, continue to the next mapping.
                if(identifierList.size() > 1)
                    continue;

                // 5.4.2)
                // Use the Issue Identifier algorithm, passing canonical issuer and the single blank
                // node identifier in identifier list, identifier, to issue a canonical replacement
                // identifier for identifier.
                std::string identifier = identifierList[0];
                std::string replacementIdentifier = state.canonical_issuer.get(identifier);

                // 5.4.3)
                // Remove identifier from non-normalized identifiers.
                nonNormalizedIdentifiers.erase(
                        std::find(nonNormalizedIdentifiers.begin(), nonNormalizedIdentifiers.end(), identifier));

                // 5.4.4)
                // Remove hash from the hash to blank nodes map.
                state.hash_to_bnodes.erase(hash);

                // 5.4.5)
                // Set simple to true.
                simple = true;
            }
        }

        // 6)
        // For each hash to identifier list mapping in hash to blank nodes map,
        // lexicographically-sorted by hash:
        std::vector<std::string> hashes;
        hashes.reserve(state.hash_to_bnodes.size());
        for (auto & hash_to_bnode : state.hash_to_bnodes) {
            hashes.push_back(hash_to_bnode.first);
        }
        for(const auto& hash : hashes) {
            std::vector<std::string> identifierList = state.hash_to_bnodes[hash];

            // 6.1)
            // Create hash path list where each item will be a result of running the Hash
            // N-Degree Quads algorithm.
            std::vector<HashNDegreeQuadsResult> hashPathList;

            // 6.2)
            // For each blank node identifier identifier in identifier list:
            for(const auto& identifier : identifierList) {
                // 6.2.1)
                // If a canonical identifier has already been issued for identifier, continue
                // to the next identifier.
                if(state.canonical_issuer.exists(identifier))
                    continue;

                // 6.2.2)
                // Create temporary issuer, an identifier issuer initialized with the prefix _:b.
                BlankNodeNames temporaryIssuer;

                // 6.2.3)
                // Use the Issue Identifier algorithm, passing temporary issuer and identifier, to
                // issue a new temporary blank node identifier for identifier.
                temporaryIssuer.get(identifier); // result ignored for now

                // 6.2.4)
                // Run the Hash N-Degree Quads algorithm, passing temporary issuer, and append the
                // result to the hash path list.
                hashPathList.push_back(hashNDegreeQuads(state, identifier, temporaryIssuer));
            }

            // 6.3)
            // For each result in the hash path list, lexicographically-sorted by the hash in result:
            std::sort(hashPathList.begin(), hashPathList.end(),
                      [](const HashNDegreeQuadsResult& a, const HashNDegreeQuadsResult& b) {
                          return a.hash < b.hash;
                      });
            for(auto result : hashPathList ) {
                // 6.3.1)
                // For each blank node identifier, existing identifier, that was issued a
                // temporary identifier by identifier issuer in result, issue a canonical
                // identifier, in the same order, using the Issue Identifier algorithm, passing
                // canonical issuer and existing identifier.
                auto temporaryIdentifiers = result.issuerUsed.getKeys();
                for( const auto& existingIdentifier : temporaryIdentifiers) {
                    state.canonical_issuer.get(existingIdentifier);
                }
            }
        }

        RDF::RDFDataset canonicalizedDataset(options);

        // 7)
        // For each quad, quad, in input dataset:
        for(const auto& quad : quads) {

            // 7.1)
            // Create a copy, quad copy, of quad and replace any existing blank node identifiers
            // using the canonical identifiers previously issued by canonical issuer.
            RDF::RDFQuad quadCopy = quad;
            state.replaceNonCanonicalIdentifiers(quadCopy.getSubject());
            state.replaceNonCanonicalIdentifiers(quadCopy.getObject());
            state.replaceNonCanonicalIdentifiers(quadCopy.getGraph());

            // 7.2)
            // Add quad copy to the normalized dataset.
            RDF::RDFTriple triple{quadCopy.getSubject(), quadCopy.getPredicate(), quadCopy.getObject()};
            if(quadCopy.getGraph())
                canonicalizedDataset.addTripleToGraph(quadCopy.getGraph()->getValue(), triple);
            else
                canonicalizedDataset.addTripleToGraph(JsonLdConsts::DEFAULT, triple);
        }

        // copy issued identifiers if requested
        if(issuedIdentifiersMap) {
            nlohmann::json idMap;
            for (const std::string &key: state.canonical_issuer.getKeys()) {
                // copy keys and values from canonical_issuer, but strip off the BLANK_NODE_PREFIX
                std::string k = key, i = state.canonical_issuer.get(key);
                if (k.find(JsonLdConsts::BLANK_NODE_PREFIX) == 0) {
                    k = k.substr(sizeof(JsonLdConsts::BLANK_NODE_PREFIX) - 1);
                }
                if (i.find(JsonLdConsts::BLANK_NODE_PREFIX) == 0) {
                    i = i.substr(sizeof(JsonLdConsts::BLANK_NODE_PREFIX) - 1);
                }
                idMap[k] = i;
            }
            *issuedIdentifiersMap = idMap;
        }

        // 8)
        // Return the normalized dataset.
        return canonicalizedDataset;
    }

}

std::string RDFCanonicalizationProcessor::canonicalize(const RDF::RDFDataset& dataset, const JsonLdOptions& options, nlohmann::json *issuedIdentifiersMap) {

    return NQuadsSerialization::toNQuads(::canonicalizeDataset(dataset, options, issuedIdentifiersMap));

}
