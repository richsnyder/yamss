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

FIND_PROGRAM(Hermes_EXECUTABLE NAMES hermes)
FIND_PATH(Hermes_INCLUDE_DIR NAMES hermes.hpp)
FIND_LIBRARY(Hermes_LIBRARY NAMES hermes_cpp)

IF(Hermes_EXECUTABLE)
  EXECUTE_PROCESS(COMMAND ${Hermes_EXECUTABLE} "--version"
      ERROR_VARIABLE Hermes_VERSION
      OUTPUT_VARIABLE Hermes_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE)
  STRING(REGEX
      REPLACE "^hermes, +version +([.0-9]+)$" "\\1"
      Hermes_VERSION
      "${Hermes_VERSION}")
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Hermes
  FOUND_VAR Hermes_FOUND
  REQUIRED_VARS Hermes_LIBRARY Hermes_INCLUDE_DIR Hermes_EXECUTABLE
  VERSION_VAR Hermes_VERSION)

IF(Hermes_FOUND)
  SET(Hermes_INCLUDE_DIRS ${Hermes_INCLUDE_DIR} ${ZeroMQ_INCLUDE_DIRS})
  SET(Hermes_LIBRARIES ${Hermes_LIBRARY} ${ZeroMQ_LIBRARIES})
ENDIF(Hermes_FOUND)

MARK_AS_ADVANCED(Hermes_EXECUTABLE Hermes_INCLUDE_DIR Hermes_LIBRARY)
