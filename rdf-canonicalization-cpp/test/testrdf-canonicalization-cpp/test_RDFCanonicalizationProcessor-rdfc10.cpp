#pragma ide diagnostic ignored "cert-err58-cpp"
#include <rdf-canonicalization-cpp/CanonicalizationProcessor.h>
#include <jsonld-cpp/JsonLdError.h>
#include <jsonld-cpp/RemoteDocument.h>
#include <jsonld-cpp/NQuadsSerialization.h>
#include "UrdnaManifestLoader.h"

#include <gtest/gtest.h>

using nlohmann::json;

// test suite fixture class
class JsonLdProcessorCanonicalizeRdfc10Test : public ::testing::Test {
public:
    static void SetUpTestCase() {
        std::string resource_dir = TEST_RESOURCE_DIR;
        UrdnaManifestLoader manifestLoader(
                resource_dir + "test_data/",
                mainManifestName);
        testCases = manifestLoader.load();
    }

    // resources shared by all tests.
    static std::string mainManifestName;
    static std::map<std::string, TestCase> testCases;

    static void performCanonicalizeTest(TestCase &testCase) {
        JsonLdOptions options = testCase.getOptions();

        std::cout << "Id: " << testCase.id << std::endl;
        std::cout << "Name: " << testCase.name << std::endl;
        if(!testCase.options.specVersion.empty())
            std::cout << "SpecVersion: " << testCase.options.specVersion << std::endl;

        std::string canonicalized;
        nlohmann::json issuedIdentifiersMap;
        try {
            canonicalized = CanonicalizationProcessor::canonicalize(testCase.input, options, &issuedIdentifiersMap);
        }
        catch (JsonLdError &e) {
            std::cout << "JsonLdError: " << e.what() << std::endl;
            if(testCase.type.count("rdfc:RDFC10NegativeEvalTest")) {
                std::string error = e.what();
                if(error.find(testCase.expectErrorCode) != std::string::npos) {
                    SUCCEED();
                    return;
                }
            }
            FAIL() << e.what();
        }
        catch (std::runtime_error &e) {
            FAIL() << e.what();
        }

        std::unique_ptr<RemoteDocument> expectedDocument =
                options.getDocumentLoader()->loadDocument(testCase.expect);

        if(expectedDocument->getContentType() == MediaType::n_quads()) {
            const std::string &expected =
                    NQuadsSerialization::toNQuads(expectedDocument->getRDFContent());
            EXPECT_EQ(canonicalized, expected);
        }
        else if(expectedDocument->getContentType() == MediaType::json()) {
            const json &expected = expectedDocument->getJSONContent();
            std::cout << "expected: " << expected.dump() << "\n";
            std::cout << "actual  : " << issuedIdentifiersMap.dump() << "\n";
            EXPECT_TRUE(expected.size() == 0 || expected == issuedIdentifiersMap);
        }

    }

    static void performCanonicalizeTestFromAlternateManifest(const std::string& testName, const std::string& manifestName) {

        std::string resource_dir = TEST_RESOURCE_DIR;
        UrdnaManifestLoader manifestLoader(
                resource_dir + "test_data/",
                manifestName);
        std::map<std::string, TestCase> localTestCases = manifestLoader.load();

        auto testCase = localTestCases.at(testName);

        performCanonicalizeTest(testCase);
    }

    void performCanonicalizeTestFromManifest(const std::string& testName, const std::string& manifestName= "") {

        if(!manifestName.empty())
            return performCanonicalizeTestFromAlternateManifest(testName, manifestName);

        auto testCase = testCases.at(testName);

        performCanonicalizeTest(testCase);
    }

};

std::string JsonLdProcessorCanonicalizeRdfc10Test::mainManifestName = "canonicalize-manifest-rdfc10.jsonld";
std::map<std::string, TestCase> JsonLdProcessorCanonicalizeRdfc10Test::testCases;


TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test001c) {
    performCanonicalizeTestFromManifest("#test001c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test002c) {
    performCanonicalizeTestFromManifest("#test002c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test003c) {
    performCanonicalizeTestFromManifest("#test003c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test004c) {
    performCanonicalizeTestFromManifest("#test004c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test005c) {
    performCanonicalizeTestFromManifest("#test005c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test006c) {
    performCanonicalizeTestFromManifest("#test006c");
}

// #test007c does not exist

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test008c) {
    performCanonicalizeTestFromManifest("#test008c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test009c) {
    performCanonicalizeTestFromManifest("#test009c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test010c) {
    performCanonicalizeTestFromManifest("#test010c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test011c) {
    performCanonicalizeTestFromManifest("#test011c");
}

// #test012c does not exist

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test013c) {
    performCanonicalizeTestFromManifest("#test013c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test014c) {
    performCanonicalizeTestFromManifest("#test014c");
}

// #test0015c does not exist

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test016c) {
    performCanonicalizeTestFromManifest("#test016c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test017c) {
    performCanonicalizeTestFromManifest("#test017c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test018c) {
    performCanonicalizeTestFromManifest("#test018c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test019c) {
    performCanonicalizeTestFromManifest("#test019c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test020c) {
    performCanonicalizeTestFromManifest("#test020c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test021c) {
    performCanonicalizeTestFromManifest("#test021c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test022c) {
    performCanonicalizeTestFromManifest("#test022c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test023c) {
    performCanonicalizeTestFromManifest("#test023c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test024c) {
    performCanonicalizeTestFromManifest("#test024c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test025c) {
    performCanonicalizeTestFromManifest("#test025c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test026c) {
    performCanonicalizeTestFromManifest("#test026c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test027c) {
    performCanonicalizeTestFromManifest("#test027c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test028c) {
    performCanonicalizeTestFromManifest("#test028c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test029c) {
    performCanonicalizeTestFromManifest("#test029c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test030c) {
    performCanonicalizeTestFromManifest("#test030c");
}

// #test0031c does not exist

// #test0032c does not exist

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test033c) {
    performCanonicalizeTestFromManifest("#test033c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test034c) {
    performCanonicalizeTestFromManifest("#test034c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test035c) {
    performCanonicalizeTestFromManifest("#test035c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test036c) {
    performCanonicalizeTestFromManifest("#test036c");
}

// #test037c does not exist

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test038c) {
    performCanonicalizeTestFromManifest("#test038c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test039c) {
    performCanonicalizeTestFromManifest("#test039c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test040c) {
    performCanonicalizeTestFromManifest("#test040c");
}

// #test0041c does not exist

// #test0042c does not exist

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test043c) {
    performCanonicalizeTestFromManifest("#test043c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test044c) {
    performCanonicalizeTestFromManifest("#test044c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test045c) {
    performCanonicalizeTestFromManifest("#test045c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test046c) {
    performCanonicalizeTestFromManifest("#test046c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test047c) {
    performCanonicalizeTestFromManifest("#test047c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test048c) {
    performCanonicalizeTestFromManifest("#test048c");
}

// #test0049c - #test0052c do not exist

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test053c) {
    performCanonicalizeTestFromManifest("#test053c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test054c) {
    performCanonicalizeTestFromManifest("#test054c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test055c) {
    performCanonicalizeTestFromManifest("#test055c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test056c) {
    performCanonicalizeTestFromManifest("#test056c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test057c) {
    performCanonicalizeTestFromManifest("#test057c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test058c) {
    performCanonicalizeTestFromManifest("#test058c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test059c) {
    performCanonicalizeTestFromManifest("#test059c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test060c) {
    performCanonicalizeTestFromManifest("#test060c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test061c) {
    performCanonicalizeTestFromManifest("#test061c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test062c) {
    performCanonicalizeTestFromManifest("#test062c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test063c) {
    performCanonicalizeTestFromManifest("#test063c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test064c) {
    performCanonicalizeTestFromManifest("#test064c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test065c) {
    performCanonicalizeTestFromManifest("#test065c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test066c) {
    performCanonicalizeTestFromManifest("#test066c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test067c) {
    performCanonicalizeTestFromManifest("#test067c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test068c) {
    performCanonicalizeTestFromManifest("#test068c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test069c) {
    performCanonicalizeTestFromManifest("#test069c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test070c) {
    performCanonicalizeTestFromManifest("#test070c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test071c) {
    performCanonicalizeTestFromManifest("#test071c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test072c) {
    performCanonicalizeTestFromManifest("#test072c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test073c) {
    performCanonicalizeTestFromManifest("#test073c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test074c) {
    performCanonicalizeTestFromManifest("#test074c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test075c) {
    performCanonicalizeTestFromManifest("#test075c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test076c) {
    performCanonicalizeTestFromManifest("#test076c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test077c) {
    performCanonicalizeTestFromManifest("#test077c");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test003m) {
    performCanonicalizeTestFromManifest("#test003m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test004m) {
    performCanonicalizeTestFromManifest("#test004m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test005m) {
    performCanonicalizeTestFromManifest("#test005m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test016m) {
    performCanonicalizeTestFromManifest("#test016m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test017m) {
    performCanonicalizeTestFromManifest("#test017m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test018m) {
    performCanonicalizeTestFromManifest("#test018m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test020m) {
    performCanonicalizeTestFromManifest("#test020m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test030m) {
    performCanonicalizeTestFromManifest("#test030m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test047m) {
    performCanonicalizeTestFromManifest("#test047m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test048m) {
    performCanonicalizeTestFromManifest("#test048m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test053m) {
    performCanonicalizeTestFromManifest("#test053m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test055m) {
    performCanonicalizeTestFromManifest("#test055m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test056m) {
    performCanonicalizeTestFromManifest("#test056m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test057m) {
    performCanonicalizeTestFromManifest("#test057m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test060m) {
    performCanonicalizeTestFromManifest("#test060m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test063m) {
    performCanonicalizeTestFromManifest("#test063m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test070m) {
    performCanonicalizeTestFromManifest("#test070m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test071m) {
    performCanonicalizeTestFromManifest("#test071m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test072m) {
    performCanonicalizeTestFromManifest("#test072m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test073m) {
    performCanonicalizeTestFromManifest("#test073m");
}

TEST_F(JsonLdProcessorCanonicalizeRdfc10Test, canonicalize_rdfc10_test075m) {
    performCanonicalizeTestFromManifest("#test075m");
}
