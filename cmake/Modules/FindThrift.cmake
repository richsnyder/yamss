# ==============================================================================
#
# Find the Apache Thrift library
#
# ------------------------------------------------------------------------------
#
# Variables defined by this module include:
#   Thrift_FOUND            If false, do not try to use Thrift
#   Thrift_INCLUDE_DIRS     Where to find the header files needed to use Thrift
#   Thrift_LIBRARIES        The libraries needed to use Thrift
#   Thrift_VERSION          The version of the Thrift library
# Also defined, but not for general use, are:
#   Thrift_INCLUDE_DIR      Where to find the Thrift header files
#   Trhfit_LIBRARY          Where to find the Thrift library
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

FIND_PROGRAM(Thrift_EXECUTABLE NAMES thrift)
FIND_PATH(Thrift_INCLUDE_DIR thrift/Thrift.h)
FIND_LIBRARY(Thrift_LIBRARY NAMES thrift)

IF(Thrift_EXECUTABLE)
  EXECUTE_PROCESS(COMMAND ${Thrift_EXECUTABLE} "-version"
  OUTPUT_VARIABLE Thrift_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE)
  STRING(REGEX REPLACE "Thrift version" "" Thrift_VERSION ${Thrift_VERSION})
  STRING(STRIP ${Thrift_VERSION} Thrift_VERSION)
ENDIF(Thrift_EXECUTABLE)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Thrift
  FOUND_VAR Thrift_FOUND
  REQUIRED_VARS Thrift_EXECUTABLE Thrift_LIBRARY Thrift_INCLUDE_DIR
  VERSION_VAR Thrift_VERSION)

IF(Thrift_FOUND)
  SET(Thrift_INCLUDE_DIRS ${Thrift_INCLUDE_DIR})
  SET(Thrift_LIBRARIES ${Thrift_LIBRARY})
ENDIF(Thrift_FOUND)

MARK_AS_ADVANCED(Thrift_INCLUDE_DIR Thrift_LIBRARY)
