cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(rdf-canonicalization-cpp-examples VERSION ${RDFCANONCPP_VERSION}
        LANGUAGES CXX )

find_package(jsonld-cpp 0.5.0 CONFIG REQUIRED)
find_package(rdf-canonicalization-cpp ${RDFCANONCPP_VERSION} CONFIG REQUIRED)


add_executable(jsonld2canonrdf jsonld2canonrdf.cpp)

set_target_properties(jsonld2canonrdf PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(jsonld2canonrdf rdf-canonicalization-cpp::rdf-canonicalization-cpp jsonld-cpp::jsonld-cpp)

