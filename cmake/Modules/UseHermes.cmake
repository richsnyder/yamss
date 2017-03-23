# ==============================================================================
#
# Hermes use module for CMake
#
# ------------------------------------------------------------------------------
#
# Functions for working with the Hermes RPC framework.
#
# ------------------------------------------------------------------------------
#
# This module defines the following functions:
#   ADD_HERMES()
#     - Add a target that generates source code from a Hermes IDL file.
#
# ==============================================================================

INCLUDE(CMakeParseArguments)
FIND_PACKAGE(Hermes REQUIRED)

# ------------------------------------------------------------------------------
#
# HERMES_CPP_FILES(INTERFACE_DEFINITION <file>
#                  SOURCE_DIRECTORY <directory>
#                  GENERATED_HEADERS <headers-variable>
#                  GENERATED_SOURCES <sources-variable>)
#
# Infer generated C++ header and source files from a Hermes IDL file.
#
# Get lists of the header and source files that will be generated for C++ from
# a Hermes interface definition file.
#
# ------------------------------------------------------------------------------

FUNCTION(HERMES_CPP_FILES)

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
    MESSAGE(FATAL_ERROR "A Hermes interface definition file is required")
  ENDIF()
  IF(NOT DEFINED A_SOURCE_DIRECTORY)
    MESSAGE(FATAL_ERROR "An output directory for generated source is required")
  ENDIF()

  # Get information about the interface definition.
  GET_FILENAME_COMPONENT(basename "${A_INTERFACE_DEFINITION}" NAME_WE)

  # Build lists of the Hermes-generated header and source code files.
  LIST(APPEND headers "${A_SOURCE_DIRECTORY}/${basename}.hpp")
  LIST(APPEND sources "${A_SOURCE_DIRECTORY}/${basename}.cpp")

  # Return the lists of the generated header and source files.
  SET(${A_GENERATED_HEADERS} ${headers} PARENT_SCOPE)
  SET(${A_GENERATED_SOURCES} ${sources} PARENT_SCOPE)

ENDFUNCTION(HERMES_CPP_FILES)

# ------------------------------------------------------------------------------
#
# HERMES_FORTRAN_FILES(INTERFACE_DEFINITION <file>
#                      SOURCE_DIRECTORY <directory>
#                      GENERATED_SOURCES <sources-variable>)
#
# Infer generated Fortran source files from a Hermes IDL file.
#
# Get a list of the source files that will be generated for Fortran from a
# Hermes interface definition file.
#
# ------------------------------------------------------------------------------

FUNCTION(HERMES_FORTRAN_FILES)

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
    MESSAGE(FATAL_ERROR "A Hermes interface definition file is required")
  ENDIF()
  IF(NOT DEFINED A_SOURCE_DIRECTORY)
    MESSAGE(FATAL_ERROR "An output directory for generated source is required")
  ENDIF()

  # Get information about the interface definition.
  GET_FILENAME_COMPONENT(basename "${A_INTERFACE_DEFINITION}" NAME_WE)

  # Build lists of the Hermes-generated source code files.
  LIST(APPEND sources ${A_SOURCE_DIRECTORY}/${basename}.f90)

  # Return the list of the generated source files.
  SET(${A_GENERATED_SOURCES} ${sources} PARENT_SCOPE)

ENDFUNCTION(HERMES_FORTRAN_FILES)

# ------------------------------------------------------------------------------
#
# HERMES_PYTHON_FILES(INTERFACE_DEFINITION <file>
#                     SOURCE_DIRECTORY <directory>
#                     GENERATED_SOURCES <sources-variable>)
#
# Infer generated Python source files from a Hermes IDL file.
#
# Get a list of the script files that will be generated for Python from a
# Hermes interface definition file.
#
# ------------------------------------------------------------------------------

FUNCTION(HERMES_PYTHON_FILES)

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
    MESSAGE(FATAL_ERROR "A Hermes interface definition file is required")
  ENDIF()
  IF(NOT DEFINED A_SOURCE_DIRECTORY)
    MESSAGE(FATAL_ERROR "An output directory for generated source is required")
  ENDIF()

  # Get information about the interface definition.
  GET_FILENAME_COMPONENT(basename "${A_INTERFACE_DEFINITION}" NAME_WE)

  # Build a list of the Hermes-generated files.
  LIST(APPEND sources ${A_SOURCE_DIRECTORY}/${basename}.py)

  # Return the list of the generated source files.
  SET(${A_GENERATED_SOURCES} ${sources} PARENT_SCOPE)

ENDFUNCTION(HERMES_PYTHON_FILES)

# ------------------------------------------------------------------------------
#
# ADD_HERMES(<target>
#            INTERFACE_DEFINITION <file>
#            SOURCE_DIRECTORY <directory>
#            [IMPORT_DIRECTORIES <import-dir-1> <import-dir-2> ...]
#            [GENERATED_HEADERS <headers-variable>]
#            [GENERATED_SOURCES <sources-variable>]
#            [GENERATOR <generator>])
#
# Adds a target that generates source code from a Hermes IDL file.
#
# Add a custom command that will execute the Hermes compiler to translate an
# interface definition <file> into code.  Paths to the generated header and
# source files can be captured in <headers-variable> and <sources-variable> and
# added to a target to enable RPCs.  If the interface definition file does not
# reside in the current source directory, <directory> can be used to specify
# its location.  Imported files will be found in the current source directory
# or any of the <import-dir-n> directories.  C++ source code will be generated
# from the interface definition file unless a <generator> is specified.
#
# ------------------------------------------------------------------------------

FUNCTION(ADD_HERMES)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
      ""
      "GENERATED_HEADERS;GENERATED_SOURCES;GENERATOR;INTERFACE_DEFINITION;SOURCE_DIRECTORY"
      "IMPORT_DIRECTORIES"
      ${ARGN})

  # Make sure that all of the required arguments are present.
  IF(NOT DEFINED A_INTERFACE_DEFINITION)
    MESSAGE(FATAL_ERROR "A Hermes interface definition file is required")
  ENDIF()
  IF(NOT DEFINED A_SOURCE_DIRECTORY)
    MESSAGE(FATAL_ERROR "An output directory for generated source is required")
  ENDIF()

  # Set the optional arguments to their default values if not present.
  IF(NOT DEFINED A_GENERATOR)
    SET(A_GENERATOR cpp)
  ENDIF(NOT DEFINED A_GENERATOR)

  # Process the interface definition file according to the selected generator.
  SET(hermes_file "${CMAKE_CURRENT_SOURCE_DIR}/${A_INTERFACE_DEFINITION}")
  IF(A_GENERATOR STREQUAL "cpp")
    HERMES_CPP_FILES(
        INTERFACE_DEFINITION ${hermes_file}
        SOURCE_DIRECTORY ${A_SOURCE_DIRECTORY}
        GENERATED_HEADERS generated_headers
        GENERATED_SOURCES generated_sources)
  ELSEIF(A_GENERATOR STREQUAL "fortran")
    HERMES_FORTRAN_FILES(
        INTERFACE_DEFINITION ${hermes_file}
        SOURCE_DIRECTORY ${A_SOURCE_DIRECTORY}
        GENERATED_SOURCES generated_sources)
  ELSEIF(A_GENERATOR STREQUAL "python")
    HERMES_PYTHON_FILES(
        INTERFACE_DEFINITION ${hermes_file}
        SOURCE_DIRECTORY ${A_SOURCE_DIRECTORY}
        GENERATED_SOURCES generated_sources)
  ELSE()
    SET(MSG "The given Hermes generator '${A_GENERATOR}' is not supported")
    MESSAGE(FATAL_ERROR ${MSG})
  ENDIF()

  # Define the options to the Hermes compiler.
  SET(hermes_options --${A_GENERATOR} --destination="${A_SOURCE_DIRECTORY}")
  FOREACH(dir ${A_IMPORT_DIRECTORIES})
    SET(hermes_options ${hermes_options} --import-path="${dir}")
  ENDFOREACH()

  # Add a custom command to run the Hermes compiler.
  ADD_CUSTOM_COMMAND(OUTPUT ${generated_headers} ${generated_sources}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${A_SOURCE_DIRECTORY}
      COMMAND ${Hermes_EXECUTABLE} ${hermes_options} ${hermes_file}
      COMMENT "Running the Hermes source generator"
      DEPENDS ${hermes_file}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  # Store lists of the generated files in output variables.
  IF(DEFINED A_GENERATED_HEADERS)
    SET(${A_GENERATED_HEADERS} ${generated_headers} PARENT_SCOPE)
  ENDIF()
  IF(DEFINED A_GENERATED_SOURCES)
    SET(${A_GENERATED_SOURCES} ${generated_sources} PARENT_SCOPE)
  ENDIF()

ENDFUNCTION(ADD_HERMES)
