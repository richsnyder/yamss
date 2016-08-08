# ==============================================================================
#
# Reporting module for YAMSS
#
# ------------------------------------------------------------------------------
#
# Functions for generating a configuration report for a YAMSS build.
#
# ------------------------------------------------------------------------------
#
# This module defines the following functions:
#   YES_OR_NO()
#     - Converts a boolean to a "yes" or "no" string.
#   VERSION_OR_NOT_FOUND()
#     - Returns the version string of a package, or "not found".
#   CONFIGURATION_REPORT()
#     - Generates a report on the configuration for a YAMSS build.
#
# ==============================================================================

INCLUDE(CMakeParseArguments)
INCLUDE(Documentation)

# ------------------------------------------------------------------------------
#
# YES_OR_NO(<variable> <boolean>)
#
# Convert a boolean to a "yes" or "no" string.
#
# If <boolean> evaluates to true, then set <variable> to "yes".  Otherwise, set
# <variable> to "no".
#
# ------------------------------------------------------------------------------

FUNCTION(YES_OR_NO A_STRING A_BOOLEAN)

  IF(${A_BOOLEAN})
    SET(${A_STRING} "yes" PARENT_SCOPE)
  ELSE()
    SET(${A_STRING} "no" PARENT_SCOPE)
  ENDIF()

ENDFUNCTION(YES_OR_NO)

# ------------------------------------------------------------------------------
#
# VERSION_OR_NOT_FOUND(VAR <variable>
#                      [PACKAGE <package>]
#                      [FOUND <found>]
#                      [VERSION <version>])
#
# Set <variable> to the version number of a software package, or "not found".
#
# If <found> is defined and evaluates to true, then set <variable> to the value
# of <version>.  Otherwise, set <variable> to "not found".  If FOUND is not
# specified, then take <found> to be <package>_FOUND.  If VERSION is not
# specified, then take <version> to be <package>_VERSION.
#
# ------------------------------------------------------------------------------

FUNCTION(VERSION_OR_NOT_FOUND)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
    ""
    "VAR;PACKAGE;FOUND;VERSION"
    ""
    ${ARGN})

  # Make sure that all of the required arguments are present.
  IF(NOT DEFINED A_VAR)
    MESSAGE(FATAL_ERROR "An output variable is required")
  ENDIF()
  IF(NOT DEFINED A_PACKAGE AND NOT DEFINED A_FOUND)
    MESSAGE(FATAL_ERROR "Either FOUND or PACKAGE must be specified")
  ENDIF()
  IF(NOT DEFINED A_PACKAGE AND NOT DEFINED A_VERSION)
    MESSAGE(FATAL_ERROR "Either VERSION or PACAKGE must be specified")
  ENDIF()

  # Set the found and version variables based on the function arguments.
  IF(A_FOUND)
    SET(found ${${A_FOUND}})
  ELSE()
    SET(found ${${A_PACKAGE}_FOUND})
  ENDIF()
  IF(A_VERSION)
    SET(version ${${A_VERSION}})
  ELSE()
    SET(version ${${A_PACKAGE}_VERSION})
  ENDIF()

  # Set the output variable based on the found and version variables.
  IF(found)
    SET(${A_VAR} ${version} PARENT_SCOPE)
  ELSE()
    SET(${A_VAR} "not found" PARENT_SCOPE)
  ENDIF()

ENDFUNCTION(VERSION_OR_NOT_FOUND)

# ------------------------------------------------------------------------------
#
# CONFIGURATION_REPORT()
#
# Generate a report on the configuration for a YAMSS build.
#
# Output a series of messages that summarize the configuration for the YAMSS
# build.
#
# ------------------------------------------------------------------------------

FUNCTION(CONFIGURATION_REPORT)

  SET(bv "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
  VERSION_OR_NOT_FOUND(VAR armadillo_version PACKAGE Armadillo)
  VERSION_OR_NOT_FOUND(VAR boost_version PACKAGE Boost VERSION bv)
  VERSION_OR_NOT_FOUND(VAR lua_version PACKAGE LUA VERSION LUA_VERSION_STRING)
  VERSION_OR_NOT_FOUND(VAR curl_version PACKAGE CURL VERSION CURL_VERSION_STRING)
  VERSION_OR_NOT_FOUND(VAR hermes_version PACKAGE Hermes)

  MESSAGE(STATUS "")
  MESSAGE(STATUS "Configuration Summary")
  MESSAGE(STATUS "---------------------")
  MESSAGE(STATUS "")
  MESSAGE(STATUS "  Armadillo version          : ${armadillo_version}")
  MESSAGE(STATUS "  Boost version              : ${boost_version}")
  MESSAGE(STATUS "  Lua version                : ${lua_version}")
  MESSAGE(STATUS "  CURL version               : ${curl_version}")
  MESSAGE(STATUS "  Hermes version             : ${hermes_version}")
  MESSAGE(STATUS "")
  DOC_SUMMARY()
  MESSAGE(STATUS "")
  MESSAGE(STATUS "  Installation prefix        : ${CMAKE_INSTALL_PREFIX}")
  MESSAGE(STATUS "")

ENDFUNCTION(CONFIGURATION_REPORT)
