# ==============================================================================
#
# Find the CZMQ high-level socket library
#
# ------------------------------------------------------------------------------
#
# Variables defined by this module include:
#   CZMQ_FOUND            If false, do not try to use CZMQ
#   CZMQ_INCLUDE_DIRS     Where to find the header files needed to use CZMQ
#   CZMQ_LIBRARIES        The libraries needed to use CZMQ
#   CZMQ_VERSION          The version of the CZMQ library
# Also defined, but not for general use, are:
#   CZMQ_INCLUDE_DIR      Where to find the CZMQ header files
#   CZMQ_LIBRARY          Where to find the CZMQ library
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

IF(CZMQ_FIND_QUIETLY)
  SET(_FIND_PACKAGE_ARG ${_FIND_PACKAGE_ARGS} QUIET)
ENDIF(CZMQ_FIND_QUIETLY)
IF(CZMQ_FIND_REQUIRED)
  SET(_FIND_PACKAGE_ARG ${_FIND_PACKAGE_ARGS} REQUIRED)
ENDIF(CZMQ_FIND_REQUIRED)

FIND_PACKAGE(ZeroMQ ${_FIND_PACKAGE_ARGS})

FIND_PATH(CZMQ_INCLUDE_DIR czmq.h)
FIND_LIBRARY(CZMQ_LIBRARY NAMES czmq)

IF(CZMQ_INCLUDE_DIR)
  SET(zmq_header "${CZMQ_INCLUDE_DIR}/czmq.h")
  FILE(STRINGS "${zmq_header}" major REGEX "define +CZMQ_VERSION_MAJOR")
  FILE(STRINGS "${zmq_header}" minor REGEX "define +CZMQ_VERSION_MINOR")
  FILE(STRINGS "${zmq_header}" patch REGEX "define +CZMQ_VERSION_PATCH")
  STRING(REGEX REPLACE ".+([0-9]+)" "\\1" major ${major})
  STRING(REGEX REPLACE ".+([0-9]+)" "\\1" minor ${minor})
  STRING(REGEX REPLACE ".+([0-9]+)" "\\1" patch ${patch})
  STRING(STRIP "${major}" major)
  STRING(STRIP "${minor}" minor)
  STRING(STRIP "${patch}" patch)
  SET(CZMQ_VERSION "${major}.${minor}.${patch}")
ENDIF(CZMQ_INCLUDE_DIR)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CZMQ
  FOUND_VAR CZMQ_FOUND
  REQUIRED_VARS
    CZMQ_LIBRARY
    CZMQ_INCLUDE_DIR
    ZeroMQ_LIBRARIES
    ZeroMQ_INCLUDE_DIRS
  VERSION_VAR CZMQ_VERSION)

IF(CZMQ_FOUND)
	SET(CZMQ_INCLUDE_DIRS
    ${CZMQ_INCLUDE_DIR}
    ${ZeroMQ_INCLUDE_DIRS})
	SET(CZMQ_LIBRARIES
    ${CZMQ_LIBRARY}
    ${ZeroMQ_LIBRARIES})
ENDIF(CZMQ_FOUND)

MARK_AS_ADVANCED(CZMQ_INCLUDE_DIR CZMQ_LIBRARY)
