# ==============================================================================
#
# Apache Thrift use module for CMake
#
# ------------------------------------------------------------------------------
#
# Functions for working with Apache Thrift.
#
# ------------------------------------------------------------------------------
#
# This module defines the following functions:
#   ADD_THRIFT()
#     - Add a target that generates a library from a Thrift IDL file.
#   THRIFT_NAMESPACE()
#     - Scans a Thrift IDL file for a language namespace.
#   THRIFT_SERVICES()
#     - Scans a Thrift IDL file for a list of services.
#
# ------------------------------------------------------------------------------
#
# Copyright (c) 2014, Richard Snyder
#
# Use, modification, and distribution is subject to the BSD License.
# See the accompanying file LICENSE.md for the full license.
# Terms can also be found at http://creativecommons.org/licenses/BSD/.
#
# ==============================================================================

INCLUDE(CMakeParseArguments)
FIND_PACKAGE(Thrift)

# ------------------------------------------------------------------------------
#
# THRIFT_SERVICES(<variable> <idl_file>)
#
# Search a Thrift IDL file for service names.
#
# Get a list of the names of all of the services defined within a file written
# in the Apache Thrift Interface Definition Language (IDL).  The parsing is
# done very simply and is probably easily confused.
#
# ------------------------------------------------------------------------------

FUNCTION(THRIFT_SERVICES A_VARIABLE A_INTERFACE_DEFINITION)

  FILE(READ ${A_INTERFACE_DEFINITION} file_contents LIMIT 16384)
  SET(service_regex "service[ \t\r\n]+([^ ]+)")
  STRING(REGEX MATCHALL "${service_regex}" regex_matches "${file_contents}")
  FOREACH(match ${regex_matches})
    STRING(REGEX REPLACE "${service_regex}" "\\1" service "${match}")
    LIST(APPEND services ${service})
  ENDFOREACH(match)
  SET(${A_VARIABLE} ${services} PARENT_SCOPE)

ENDFUNCTION(THRIFT_SERVICES)

# ------------------------------------------------------------------------------
#
# THRIFT_NAMESPACE(<variable> <language> <idl_file>)
#
# Search a Thrift IDL file for a namespace for the given language.
#
# Searches the given Thrift IDL file for a namespace declaration for the given
# language.  If found, A_VARIABLE is set equal to the given namespace.  If not
# found, then A_VARIABLE is set equal to <A_VARIABLE>-NOTFOUND.
#
# ------------------------------------------------------------------------------

FUNCTION(THRIFT_NAMESPACE A_VARIABLE A_LANGUAGE A_INTERFACE_DEFINITION)

  FILE(READ ${A_INTERFACE_DEFINITION} file_contents LIMIT 16384)
  SET(namespace_re "namespace[ \t\r\n]+${A_LANGUAGE}[ \t\r\n]+([^ \t\r\n]+)")
  STRING(REGEX MATCH "${namespace_re}" match "${file_contents}")
  IF(match)
    STRING(REGEX REPLACE "${namespace_re}" "\\1" namespace "${match}")
    SET(${A_VARIABLE} "${namespace}" PARENT_SCOPE)
  ELSE()
    SET(${A_VARIABLE} "${A_VARIABLE}-NOTFOUND" PARENT_SCOPE)
  ENDIF()

ENDFUNCTION(THRIFT_NAMESPACE)

# ------------------------------------------------------------------------------
#
# THRIFT_CPP_FILES(INTERFACE_DEFINITION <file>
#                  SOURCE_DIRECTORY <directory>
#                  GENERATED_HEADERS <headers-variable>
#                  GENERATED_SOURCES <sources-variable>)
#
# Infer generated header and source files from a Thrift IDL file.
#
# Get lists of the header and source files that will be generated for C++ from
# an Apache Thrift interface definition file.
#
# ------------------------------------------------------------------------------

FUNCTION(THRIFT_CPP_FILES)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
      ""
      "INTERFACE_DEFINITION;SOURCE_DIRECTORY;GENERATED_HEADERS;GENERATED_SOURCES"
      ""
      ${ARGN})

  # Make sure that all of the required arguments are present.
  IF(NOT DEFINED A_GENERATED_HEADERS)
    MESSAGE(FATAL_ERROR "An output variable for generated headers is required")
  ENDIF()
  IF(NOT DEFINED A_GENERATED_SOURCES)
    MESSAGE(FATAL_ERROR "An output variable for generated source is required")
  ENDIF()
  IF(NOT DEFINED A_INTERFACE_DEFINITION)
    MESSAGE(FATAL_ERROR "A Thrift interface definition file is required")
  ENDIF()
  IF(NOT DEFINED A_SOURCE_DIRECTORY)
    MESSAGE(FATAL_ERROR "An output directory for generated source is required")
  ENDIF()

  # Get information about the interface definition.
  GET_FILENAME_COMPONENT(basename "${A_INTERFACE_DEFINITION}" NAME_WE)
  THRIFT_SERVICES(services "${A_INTERFACE_DEFINITION}")

  # Build lists of the Thrift-generated header and source code files.
  LIST(APPEND headers "${A_SOURCE_DIRECTORY}/${basename}_constants.h")
  LIST(APPEND headers "${A_SOURCE_DIRECTORY}/${basename}_types.h")
  LIST(APPEND sources "${A_SOURCE_DIRECTORY}/${basename}_constants.cpp")
  LIST(APPEND sources "${A_SOURCE_DIRECTORY}/${basename}_types.cpp")
  FOREACH(service ${services})
    LIST(APPEND headers "${A_SOURCE_DIRECTORY}/${service}.h")
    LIST(APPEND sources "${A_SOURCE_DIRECTORY}/${service}.cpp")
  ENDFOREACH(service)

  # Return the lists of the generated header and source files.
  SET(${A_GENERATED_HEADERS} ${headers} PARENT_SCOPE)
  SET(${A_GENERATED_SOURCES} ${sources} PARENT_SCOPE)

ENDFUNCTION(THRIFT_CPP_FILES)

# ------------------------------------------------------------------------------
#
# THRIFT_JAVA_FILES(INTERFACE_DEFINITION <file>
#                   SOURCE_DIRECTORY <directory>
#                   GENERATED_SOURCES <sources-variable>)
#
# Infer generated source files from a Thrift IDL file.
#
# Get a list of the source files that will be generated for Java from an Apache
# Thrift interface definition file.
#
# ------------------------------------------------------------------------------

FUNCTION(THRIFT_JAVA_FILES)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
      ""
      "INTERFACE_DEFINITION;SOURCE_DIRECTORY;GENERATED_SOURCES"
      ""
      ${ARGN})

  # Make sure that all of the required arguments are present.
  IF(NOT DEFINED A_GENERATED_SOURCES)
    MESSAGE(FATAL_ERROR "An output variable for generated source is required")
  ENDIF()
  IF(NOT DEFINED A_INTERFACE_DEFINITION)
    MESSAGE(FATAL_ERROR "A Thrift interface definition file is required")
  ENDIF()
  IF(NOT DEFINED A_SOURCE_DIRECTORY)
    MESSAGE(FATAL_ERROR "An output directory for generated source is required")
  ENDIF()

  # Get information about the interface definition.
  GET_FILENAME_COMPONENT(basename "${A_INTERFACE_DEFINITION}" NAME_WE)
  THRIFT_SERVICES(services "${A_INTERFACE_DEFINITION}")
  THRIFT_NAMESPACE(namespace java "${A_INTERFACE_DEFINITION}")

  # Build lists of the Thrift-generated source code files.
  IF(namespace)
    STRING(REPLACE "." "/" namespace "${namespace}")
    SET(directory "${CMAKE_CURRENT_BINARY_DIR}/${A_SOURCE_DIRECTORY}/${namespace}")
  ELSE()
    SET(directory "${CMAKE_CURRENT_BINARY_DIR}/${A_SOURCE_DIRECTORY}")
  ENDIF()
  FOREACH(service ${services})
    LIST(APPEND sources "${directory}/${service}.java")
  ENDFOREACH(service)

  # Return the list of the generated source files.
  SET(${A_GENERATED_SOURCES} ${sources} PARENT_SCOPE)

ENDFUNCTION(THRIFT_JAVA_FILES)

# ------------------------------------------------------------------------------
#
# THRIFT_PY_FILES(INTERFACE_DEFINITION <file>
#                 SOURCE_DIRECTORY <directory>
#                 GENERATED_SOURCES <sources-variable>)
#
# Infer generated source files from a Thrift IDL file.
#
# Get a list of the script files that will be generated for Python from an
# Apache Thrift interface definition file.
#
# ------------------------------------------------------------------------------

FUNCTION(THRIFT_PY_FILES)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
      ""
      "INTERFACE_DEFINITION;SOURCE_DIRECTORY;GENERATED_SOURCES"
      ""
      ${ARGN})

  # Make sure that all of the required arguments are present
  IF(NOT DEFINED A_GENERATED_SOURCES)
    MESSAGE(FATAL_ERROR "An output variable for generated source is required")
  ENDIF()
  IF(NOT DEFINED A_INTERFACE_DEFINITION)
    MESSAGE(FATAL_ERROR "A Thrift interface definition file is required")
  ENDIF()
  IF(NOT DEFINED A_SOURCE_DIRECTORY)
    MESSAGE(FATAL_ERROR "An output directory for generated source is required")
  ENDIF()

  # Get information about the interface definition.
  GET_FILENAME_COMPONENT(basename "${A_INTERFACE_DEFINITION}" NAME_WE)
  THRIFT_SERVICES(services "${A_INTERFACE_DEFINITION}")
  THRIFT_NAMESPACE(namespace py "${A_INTERFACE_DEFINITION}")
  IF(NOT namespace)
    SET(namespace "${basename}")
  ENDIF()
  STRING(REPLACE "." "/" namespace "${namespace}")

  # Build a list of the Thrift-generated files.
  SET(directory "${A_SOURCE_DIRECTORY}/${namespace}")
  FOREACH(service ${services})
    LIST(APPEND sources "${directory}/${service}.py")
  ENDFOREACH(service)
  LIST(APPEND sources "${directory}/constants.py")
  LIST(APPEND sources "${directory}/ttypes.py")
  WHILE(NOT directory STREQUAL A_SOURCE_DIRECTORY)
    LIST(APPEND sources "${directory}/__init__.py")
    GET_FILENAME_COMPONENT(directory "${directory}" DIRECTORY)
  ENDWHILE()
  LIST(APPEND sources "${A_SOURCE_DIRECTORY}/__init__.py")

  # Return the list of the generated source files.
  SET(${A_GENERATED_SOURCES} ${sources} PARENT_SCOPE)

ENDFUNCTION(THRIFT_PY_FILES)

# ------------------------------------------------------------------------------
#
# ADD_THRIFT(<target>
#            INTERFACE_DEFINITION <file>
#            SOURCE_DIRECTORY <directory>
#            [GENERATED_HEADERS <headers-variable>]
#            [GENERATED_SOURCES <sources-variable>]
#            [GENERATOR <generator>])
#
# TBD.
#
# ------------------------------------------------------------------------------

FUNCTION(ADD_THRIFT)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
      ""
      "GENERATED_HEADERS;GENERATED_SOURCES;GENERATOR;INTERFACE_DEFINITION;SOURCE_DIRECTORY"
      ""
      ${ARGN})

  # Make sure that all of the required arguments are present.
  IF(NOT DEFINED A_INTERFACE_DEFINITION)
    MESSAGE(FATAL_ERROR "A Thrift interface definition file is required")
  ENDIF()
  IF(NOT DEFINED A_SOURCE_DIRECTORY)
    MESSAGE(FATAL_ERROR "An output directory for generated source is required")
  ENDIF()

  # Set the optional arguments to their default values if not present.
  IF(NOT DEFINED A_GENERATOR)
    SET(A_GENERATOR cpp)
  ENDIF(NOT DEFINED A_GENERATOR)

  # Process the interface definition file according to the selected generator.
  SET(thrift_file "${CMAKE_CURRENT_SOURCE_DIR}/${A_INTERFACE_DEFINITION}")
  IF(A_GENERATOR STREQUAL "cpp")
    THRIFT_CPP_FILES(
        INTERFACE_DEFINITION ${thrift_file}
        SOURCE_DIRECTORY ${A_SOURCE_DIRECTORY}
        GENERATED_HEADERS generated_headers
        GENERATED_SOURCES generated_sources)
  ELSEIF(A_GENERATOR STREQUAL "java")
    THRIFT_JAVA_FILES(
        INTERFACE_DEFINITION ${thrift_file}
        SOURCE_DIRECTORY ${A_SOURCE_DIRECTORY}
        GENERATED_SOURCES generated_sources)
  ELSEIF(A_GENERATOR STREQUAL "py")
    THRIFT_PY_FILES(
        INTERFACE_DEFINITION ${thrift_file}
        SOURCE_DIRECTORY ${A_SOURCE_DIRECTORY}
        GENERATED_SOURCES generated_sources)
  ELSE()
    SET(MSG "The given Thrift generator '${A_GENERATOR}' is not supported")
    MESSAGE(FATAL_ERROR ${MSG})
  ENDIF()

  # Add a custom command to run the Thrift compiler.
  SET(thrift_options -gen ${A_GENERATOR} -out "${A_SOURCE_DIRECTORY}")
  ADD_CUSTOM_COMMAND(OUTPUT ${generated_headers} ${generated_sources}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${A_SOURCE_DIRECTORY}
      COMMAND ${Thrift_EXECUTABLE} ${thrift_options} ${thrift_file}
      COMMENT "Running Apache Thrift"
      DEPENDS ${thrift_file}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  # Store lists of the generated files in output variables.
  IF(DEFINED A_GENERATED_HEADERS)
    SET(${A_GENERATED_HEADERS} ${generated_headers} PARENT_SCOPE)
  ENDIF()
  IF(DEFINED A_GENERATED_SOURCES)
    SET(${A_GENERATED_SOURCES} ${generated_sources} PARENT_SCOPE)
  ENDIF()

ENDFUNCTION(ADD_THRIFT)
