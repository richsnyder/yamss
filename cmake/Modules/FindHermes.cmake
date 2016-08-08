# ==============================================================================
#
# Find the Hermes RPC framework
#
# ------------------------------------------------------------------------------
#
# Variables defined by this module include:
#   Hermes_FOUND            If false, do not try to use Hermes
#   Hermes_INCLUDE_DIRS     Where to find the header files needed to use Hermes
#   Hermes_LIBRARIES        The libraries needed to use Hermes
#   Hermes_EXECUTABLE       The path of the Hermes source generator
# Also defined, but not for general use, are:
#   Hermes_INCLUDE_DIR      Where to find the Hermes header files
#   Hermes_LIBRARY          Where to find the Hermes library
#
# ==============================================================================

IF(Hermes_FIND_QUIETLY)
  SET(_FIND_PACKAGE_ARGS ${_FIND_PACKAGE_ARGS} QUIET)
ENDIF()
IF(Hermes_FIND_REQUIRED)
  SET(_FIND_PACKAGE_ARGS ${_FIND_PACKAGE_ARGS} REQUIRED)
ENDIF()

FIND_PACKAGE(ZeroMQ ${_FIND_PACKAGE_ARGS})

FIND_PATH(Hermes_INCLUDE_DIR hermes.hpp)
FIND_LIBRARY(Hermes_LIBRARY NAMES hermes_cpp)
FIND_PROGRAM(Hermes_EXECUTABLE NAMES hermes)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Hermes
  FOUND_VAR Hermes_FOUND
  REQUIRED_VARS Hermes_LIBRARY Hermes_INCLUDE_DIR Hermes_EXECUTABLE)

IF(Hermes_FOUND)
  SET(Hermes_INCLUDE_DIRS ${Hermes_INCLUDE_DIR})
  SET(Hermes_LIBRARIES ${Hermes_LIBRARY})
ENDIF(Hermes_FOUND)

MARK_AS_ADVANCED(Hermes_INCLUDE_DIR Hermes_LIBRARY)
