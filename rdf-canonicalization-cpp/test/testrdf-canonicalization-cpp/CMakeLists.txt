# Turn off some warnings to silence issues coming from googletest code
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  add_definitions(-Wno-weak-vtables -Wno-exit-time-destructors -Wno-global-constructors)
endif()

#
# Build common test files into lib so they only need to be compiled once
#
add_library(
    librdf-canonicalization-cpp_test STATIC
    main.cpp
    ManifestLoader.cpp
    ManifestLoader.h
    UrdnaManifestLoader.cpp
    UrdnaManifestLoader.h
    TestCase.cpp
    TestCase.h
    UriBaseRewriter.cpp
    UriBaseRewriter.h
    TestCaseOptions.cpp
    TestCaseOptions.h
)

target_link_libraries(
    librdf-canonicalization-cpp_test
    rdf-canonicalization-cpp::rdf-canonicalization-cpp
    gtest
    rapidcheck_gtest
)

#
# rdf-canonicalization-cpp
#
add_executable(
    UnitTests_rdf-canonicalization-cpp
    test_Permutator.cpp
    test_Sha256.cpp
    test_Sha384.cpp
)

target_compile_features(UnitTests_rdf-canonicalization-cpp PRIVATE cxx_std_11)
set_target_properties(UnitTests_rdf-canonicalization-cpp PROPERTIES CXX_EXTENSIONS OFF)
target_compile_definitions(UnitTests_rdf-canonicalization-cpp PUBLIC TEST_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")

target_link_libraries(UnitTests_rdf-canonicalization-cpp PUBLIC librdf-canonicalization-cpp_test )

add_test(NAME UnitTests_rdf-canonicalization-cpp
         COMMAND UnitTests_rdf-canonicalization-cpp)


#
# API
#
set(ALGORITHMS "urdna2015;rdfc10")

foreach(ALGO ${ALGORITHMS})
    message(STATUS "Adding ${ALGO}")
    add_executable(
        UnitTests_RDFCanonicalizationProcessor_${ALGO}_rdf-canonicalization-cpp
        test_RDFCanonicalizationProcessor-${ALGO}.cpp
    )

    target_compile_features(UnitTests_RDFCanonicalizationProcessor_${ALGO}_rdf-canonicalization-cpp PRIVATE cxx_std_11)
    set_target_properties(UnitTests_RDFCanonicalizationProcessor_${ALGO}_rdf-canonicalization-cpp PROPERTIES CXX_EXTENSIONS OFF)
    target_compile_definitions(UnitTests_RDFCanonicalizationProcessor_${ALGO}_rdf-canonicalization-cpp PUBLIC TEST_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")

    target_link_libraries(UnitTests_RDFCanonicalizationProcessor_${ALGO}_rdf-canonicalization-cpp librdf-canonicalization-cpp_test)

    add_test(NAME UnitTests_RDFCanonicalizationProcessor_${ALGO}_rdf-canonicalization-cpp
             COMMAND UnitTests_RDFCanonicalizationProcessor_${ALGO}_rdf-canonicalization-cpp)
endforeach()
