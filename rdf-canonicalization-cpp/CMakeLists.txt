cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

## set default install prefix if we are top-level project
#if(NOT WIN32 AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
#    set(CMAKE_INSTALL_PREFIX "/opt/contract.design/jsonld-cpp")
#endif()

project(rdf-canonicalization-cpp-library VERSION ${RDFCANONCPP_VERSION}
        LANGUAGES CXX )

include(GNUInstallDirs)

find_package(jsonld-cpp 0.5.0 CONFIG REQUIRED)

# Define library target

set(LIB_HEADER_FILES
        CanonicalizationProcessor.h
)

set(LIB_PRIVATE_HEADER_FILES
        detail/Permutator.h
        detail/RDFCanonicalizationProcessor.h
        detail/sha256.h
        detail/sha384.h
)

set(LIB_SOURCE_FILES
        CanonicalizationProcessor.cpp
        detail/Permutator.cpp
        detail/RDFCanonicalizationProcessor.cpp
        detail/sha256.cpp
        detail/sha384.cpp
        )

add_library(rdf-canonicalization-cpp STATIC ${LIB_HEADER_FILES} ${LIB_PRIVATE_HEADER_FILES} ${LIB_SOURCE_FILES})
add_library(rdf-canonicalization-cpp::rdf-canonicalization-cpp ALIAS rdf-canonicalization-cpp)


set_property(
        TARGET rdf-canonicalization-cpp
        PROPERTY PUBLIC_HEADER ${LIB_HEADER_FILES}
)

set_property(
        TARGET rdf-canonicalization-cpp
        PROPERTY PRIVATE_HEADER ${LIB_PRIVATE_HEADER_FILES}
)

# Set include directory

target_include_directories(rdf-canonicalization-cpp
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        )

# Link libraries

target_link_libraries(rdf-canonicalization-cpp jsonld-cpp::jsonld-cpp)

# Misc properties

target_compile_features(rdf-canonicalization-cpp PRIVATE cxx_std_14)
set_target_properties(rdf-canonicalization-cpp PROPERTIES CXX_EXTENSIONS OFF)

# Set version

target_compile_definitions(rdf-canonicalization-cpp PRIVATE -DRDFCANONCPP_VERSION=${RDFCANONCPP_VERSION})

if(RDFCANONCPP_BUILD_TESTS)
  enable_testing()
  # Set options to build googletest and rapidcheck or not. Other
  # projects that embed libjsonld-cpp and already use googletest and/or
  # rapidcheck could set these options to OFF
  option(RDFCANONCPP_BUILD_GOOGLETEST "Build googletest" ${RDFCANONCPP_BUILD_GOOGLETEST})
  option(RDFCANONCPP_BUILD_RAPIDCHECK "Build rapidcheck" ${RDFCANONCPP_BUILD_RAPIDCHECK})
  add_subdirectory(test)
endif()

if(INSTALL_RDFCANONCPP)

  # Configuration

  set(version_config "${CMAKE_CURRENT_BINARY_DIR}/rdf-canonicalization-cppConfigVersion.cmake")
  set(project_config "${CMAKE_CURRENT_BINARY_DIR}/rdf-canonicalization-cppConfig.cmake")
  set(namespace "rdf-canonicalization-cpp::")

  # Include module with function 'write_basic_package_version_file' and
  # 'configure_package_config_file'
  include(CMakePackageConfigHelpers)

  # Configure '<PROJECT-NAME>ConfigVersion.cmake'
  # Note: PROJECT_VERSION is used as a VERSION
  write_basic_package_version_file(
          "${version_config}"
          COMPATIBILITY SameMajorVersion
  )

  # Install '<PROJECT-NAME>ConfigVersion.cmake'
  install(
          FILES ${version_config}
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/rdf-canonicalization-cpp)

  # Configure '<PROJECT-NAME>Config.cmake' from .in file
  configure_package_config_file(
          "${PROJECT_SOURCE_DIR}/cmake/rdf-canonicalization-cppConfig.cmake.in"
          ${project_config}
          INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/rdf-canonicalization-cpp
  )

  # Install '<PROJECT-NAME>Config.cmake'
  install(FILES ${project_config}
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/rdf-canonicalization-cpp
          )

  # Install Targets

  install(
          EXPORT rdf-canonicalization-cpp_Targets
          FILE ${PROJECT_NAME}Targets.cmake
          NAMESPACE ${namespace}
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/rdf-canonicalization-cpp
  )


  install(
          TARGETS rdf-canonicalization-cpp
          EXPORT rdf-canonicalization-cpp_Targets
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rdf-canonicalization-cpp
  )


endif()

