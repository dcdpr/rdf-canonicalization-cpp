cmake_minimum_required(VERSION 3.14 FATAL_ERROR)


# Version settings

set(RDFCANONCPP_VERSION_MAJOR 0)
set(RDFCANONCPP_VERSION_MINOR 5)
set(RDFCANONCPP_VERSION_PATCH 0)
set(RDFCANONCPP_VERSION ${RDFCANONCPP_VERSION_MAJOR}.${RDFCANONCPP_VERSION_MINOR}.${RDFCANONCPP_VERSION_PATCH} CACHE STRING "" FORCE)

# General settings

set(RDFCANONCPP_PROJECT "rdf-canonicalization-cpp" CACHE STRING "Project Name")

# Test settings

set(RDFCANONCPP_BUILD_TESTS ON CACHE BOOL "Build test executables")
set(RDFCANONCPP_BUILD_GOOGLETEST ON CACHE BOOL "Build googletest for testing")
set(RDFCANONCPP_BUILD_RAPIDCHECK ON CACHE BOOL "Build rapidcheck for testing")

# Examples settings

set(RDFCANONCPP_BUILD_EXAMPLES ON CACHE BOOL "Build example executables")

# Install

set(INSTALL_RDFCANONCPP ON CACHE BOOL "Enable installation")


# Log settings

message(STATUS "---------------- RDFCANONCPP OPTIONS ----------------")

message(STATUS "RDFCANONCPP_PROJECT            : " ${RDFCANONCPP_PROJECT})
message(STATUS "RDFCANONCPP_VERSION            : " ${RDFCANONCPP_VERSION})

message(STATUS "RDFCANONCPP_BUILD_TESTS        : " ${RDFCANONCPP_BUILD_TESTS})
message(STATUS "RDFCANONCPP_BUILD_EXAMPLES     : " ${RDFCANONCPP_BUILD_EXAMPLES})

message(STATUS "INSTALL_RDFCANONCPP            : " ${INSTALL_RDFCANONCPP})
message(STATUS "CMAKE_BUILD_TYPE            : " ${CMAKE_BUILD_TYPE})
message(STATUS "CMAKE_INSTALL_PREFIX        : " ${CMAKE_INSTALL_PREFIX})
message(STATUS "CMAKE_TOOLCHAIN_FILE        : " ${CMAKE_TOOLCHAIN_FILE})

message(STATUS "-----------------------------------------------------")

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(installDir ${CMAKE_CURRENT_BINARY_DIR}/install)
else()
  set(installDir ${CMAKE_INSTALL_PREFIX})
endif()
message(STATUS "installDir        : " ${installDir})

# Project

project(${RDFCANONCPP_PROJECT} VERSION ${RDFCANONCPP_VERSION}
        DESCRIPTION "Digital Contract Design's C++ implementation of the RDF Canonicalization algorithm version 1.0 (RDFC-1.0)"
        HOMEPAGE_URL "https://github.com/dcdpr/rdf-canonicalization-cpp"
        LANGUAGES CXX )

find_package(jsonld-cpp 0.5.0 CONFIG REQUIRED)

# Super build...
include(ExternalProject)

# Add subdirectories for external dependencies

# ...

# Add local subdirectories

ExternalProject_Add(rdf-canonicalization-cpp-library
      SOURCE_DIR  ${CMAKE_CURRENT_LIST_DIR}/rdf-canonicalization-cpp
      INSTALL_DIR ${installDir}
      LIST_SEPARATOR | # Use an alternate list separator, using default of ; doesn't work quite right
      CMAKE_ARGS  -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                  -DCMAKE_PREFIX_PATH=<INSTALL_DIR>|${CMAKE_PREFIX_PATH}
                  -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
                  -DRDFCANONCPP_VERSION=${RDFCANONCPP_VERSION}
                  -DINSTALL_RDFCANONCPP=${INSTALL_RDFCANONCPP}
                  -DRDFCANONCPP_BUILD_TESTS=${RDFCANONCPP_BUILD_TESTS}
                  -DRDFCANONCPP_BUILD_GOOGLETEST=${RDFCANONCPP_BUILD_GOOGLETEST}
                  -DRDFCANONCPP_BUILD_RAPIDCHECK=${RDFCANONCPP_BUILD_RAPIDCHECK}
      DEPENDS     jsonld-cpp::jsonld-cpp
  )

if(RDFCANONCPP_BUILD_EXAMPLES)

ExternalProject_Add(rdf-canonicalization-cpp-examples
      SOURCE_DIR  ${CMAKE_CURRENT_LIST_DIR}/examples
      INSTALL_DIR ${installDir}
      LIST_SEPARATOR | # Use an alternate list separator, using default of ; doesn't work quite right
      CMAKE_ARGS  -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                  -DCMAKE_PREFIX_PATH:PATH=<INSTALL_DIR>|${CMAKE_PREFIX_PATH}
                  -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
                  -DRDFCANONCPP_VERSION=${RDFCANONCPP_VERSION}
      INSTALL_COMMAND     ""
      DEPENDS     rdf-canonicalization-cpp-library
  )

endif()

add_custom_target(
  test
  COMMAND ctest
  WORKING_DIRECTORY rdf-canonicalization-cpp-library-prefix/src/rdf-canonicalization-cpp-library-build/test/testrdf-canonicalization-cpp
  DEPENDS rdf-canonicalization-cpp-library)

add_custom_target(
  test-failed
  COMMAND ctest --rerun-failed --output-on-failure
  WORKING_DIRECTORY rdf-canonicalization-cpp-library-prefix/src/rdf-canonicalization-cpp-library-build/test/testrdf-canonicalization-cpp
  DEPENDS rdf-canonicalization-cpp-library)

