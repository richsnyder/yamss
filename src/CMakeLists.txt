# List all of the core source and header files.

SET(SOURCES
    about.cpp
    element.cpp
    ostream.cpp
)
SET(HEADERS
    yamss/about.hpp
    yamss/complex.hpp
    yamss/element.hpp
    yamss/eom.hpp
    yamss/input_reader.hpp
    yamss/iterate.hpp
    yamss/load.hpp
    yamss/map_values.hpp
    yamss/matrix_cast.hpp
    yamss/node.hpp
    yamss/run_simulation.hpp
    yamss/runner.hpp
    yamss/structure.hpp
    yamss/evaluator/evaluator.hpp
    yamss/evaluator/interface.hpp
    yamss/evaluator/lua.hpp
    yamss/inspector/inspector.hpp
    yamss/inspector/modes.hpp
    yamss/inspector/motion.hpp
    yamss/inspector/ostream.hpp
    yamss/inspector/ptree.hpp
    yamss/inspector/summary.hpp
    yamss/integrator/integrator.hpp
    yamss/integrator/generalized_alpha.hpp
    yamss/integrator/newmark_beta.hpp
)
SET(CONFIGURED_HEADERS
    yamss/config.hpp
    yamss/version.hpp
)
SET(EXE_SOURCES
  clp.cpp
  yamss.cpp
)
SET(EXE_HEADERS
  yamss/clp.hpp
)

# Define the Hermes target if the server mode is enabled.

IF(BUILD_SERVER)
  INCLUDE(UseHermes)
  ADD_HERMES(
      GENERATOR cpp
      INTERFACE_DEFINITION yamss.hid
      SOURCE_DIRECTORY include/yamss/server
      GENERATED_SOURCES HERMES_SOURCES
      GENERATED_HEADERS HERMES_HEADERS
  )
  SET(EXE_SOURCES ${EXE_SOURCES}
      server.cpp
      run_server.cpp
      transporter.cpp
  )
  SET(HEADERS ${HEADERS}
      yamss/run_server.hpp
      yamss/server/server.hpp
      yamss/server/transporter.hpp
  )
ENDIF(BUILD_SERVER)
INSTALL(FILES yamss.hid DESTINATION share/yamss)

# Identify the directories that contain include files.

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/include)

# Process the header files.

FOREACH(file ${HEADERS})
  GET_FILENAME_COMPONENT(subdirectory ${file} DIRECTORY)
  INSTALL(FILES ${file} DESTINATION include/${subdirectory})
ENDFOREACH(file)
FOREACH(file ${CONFIGURED_HEADERS})
  GET_FILENAME_COMPONENT(subdirectory ${file} DIRECTORY)
  SET(src ${CMAKE_CURRENT_SOURCE_DIR}/${file}.in)
  SET(dst ${CMAKE_CURRENT_BINARY_DIR}/include/${file})
  CONFIGURE_FILE(${src} ${dst})
  INSTALL(FILES ${dst} DESTINATION include/${subdirectory})
ENDFOREACH(file)

# Set options for the build.

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(FEATURES cxx_auto_type cxx_defaulted_functions)

# Define library targets.

ADD_LIBRARY(yamss-static STATIC ${SOURCES})
TARGET_COMPILE_FEATURES(yamss-static PUBLIC ${FEATURES})
SET_TARGET_PROPERTIES(yamss-static PROPERTIES OUTPUT_NAME yamss)
INSTALL(TARGETS yamss-static ARCHIVE DESTINATION lib)

ADD_LIBRARY(yamss-shared SHARED ${SOURCES})
TARGET_COMPILE_FEATURES(yamss-shared PUBLIC ${FEATURES})
TARGET_LINK_LIBRARIES(yamss-shared ${EXTRA_LIBS})
SET_TARGET_PROPERTIES(yamss-shared PROPERTIES OUTPUT_NAME yamss)
INSTALL(TARGETS yamss-shared LIBRARY DESTINATION lib)

# Define the executable target.

ADD_EXECUTABLE(yamss ${EXE_SOURCES} ${HERMES_SOURCES})
TARGET_COMPILE_FEATURES(yamss PUBLIC ${FEATURES})
TARGET_LINK_LIBRARIES(yamss yamss-shared)
INSTALL(TARGETS yamss RUNTIME DESTINATION bin)
