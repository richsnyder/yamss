CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

PROJECT(yamss LANGUAGES C CXX VERSION 2.6.2)

# Add custom modules to the module search path and mark them for installation.

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

# Include CMake files.

INCLUDE(Documentation)
INCLUDE(Reporting)

# Locate all of the required dependencies.

FIND_PACKAGE(Armadillo REQUIRED)
INCLUDE_DIRECTORIES(${Armadillo_INCLUDE_DIRS})
LIST(APPEND EXTRA_LIBS ${Armadillo_LIBRARIES})

SET(BOOST_COMPONENTS filesystem program_options system)
FIND_PACKAGE(Boost 1.55.0 COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LIST(APPEND EXTRA_LIBS ${Boost_LIBRARIES})

FIND_PACKAGE(Lua REQUIRED)
INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
LIST(APPEND EXTRA_LIBS ${LUA_LIBRARIES})

# Report on the build configuration.

CONFIGURATION_REPORT()

# Add the project subdirectories.

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(doc)
