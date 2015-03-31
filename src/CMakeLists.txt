# List all of the core source and header files.

SET(YAMSS_SOURCES
    about.cpp
    clp.cpp
    element.cpp
    run_server.cpp
    yamss.cpp
)
SET(YAMSS_HEADERS
    yamss/about.hpp
    yamss/clp.hpp
    yamss/complex.hpp
    yamss/element.hpp
    yamss/eom.hpp
    yamss/input_reader.hpp
    yamss/iterate.hpp
    yamss/load.hpp
    yamss/map_values.hpp
    yamss/matrix_cast.hpp
    yamss/node.hpp
    yamss/run_server.hpp
    yamss/run_simulation.hpp
    yamss/runner.hpp
    yamss/structure.hpp
    yamss/evaluator/evaluator.hpp
    yamss/evaluator/lua.hpp
    yamss/inspector/inspector.hpp
    yamss/inspector/modes.hpp
    yamss/inspector/motion.hpp
    yamss/inspector/ptree.hpp
    yamss/inspector/stream_inspector.hpp
    yamss/inspector/summary.hpp
    yamss/integrator/integrator.hpp
    yamss/integrator/generalized_alpha.hpp
    yamss/integrator/newmark_beta.hpp
)
SET(YAMSS_CONFIG_HEADERS
    yamss/config.hpp
    yamss/version.hpp
)

# Define the Thrift target if the server mode is enabled.

IF(BUILD_SERVER)
  INCLUDE(UseThrift)
  ADD_THRIFT(
      GENERATOR cpp
      INTERFACE_DEFINITION yamss.thrift
      SOURCE_DIRECTORY include/yamss/server
      GENERATED_SOURCES THRIFT_SOURCES
      GENERATED_HEADERS THRIFT_HEADERS)
  SET(YAMSS_SOURCES ${YAMSS_SOURCES}
      handler.cpp
      transporter.cpp
  )
  SET(YAMSS_HEADERS ${YAMSS_HEADERS}
      yamss/server/handler.hpp
      yamss/server/transporter.hpp
  )
ENDIF(BUILD_SERVER)
INSTALL(FILES yamss.thrift DESTINATION share)

# Identify the directories that contain include files.

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/include)

# Process the header files.

FOREACH(file ${YAMSS_HEADERS})
  GET_FILENAME_COMPONENT(subdirectory ${file} DIRECTORY)
  #INSTALL(FILES ${file} DESTINATION include/${subdirectory})
ENDFOREACH(file)
FOREACH(file ${YAMSS_CONFIG_HEADERS})
  GET_FILENAME_COMPONENT(subdirectory ${file} DIRECTORY)
  SET(src ${CMAKE_CURRENT_SOURCE_DIR}/${file}.in)
  SET(dst ${CMAKE_CURRENT_BINARY_DIR}/include/${file})
  CONFIGURE_FILE(${src} ${dst})
  #INSTALL(FILES ${dst} DESTINATION include/${subdirectory})
ENDFOREACH(file)
FOREACH(file ${THRIFT_HEADERS})
  GET_FILENAME_COMPONENT(directory ${file} DIRECTORY)
  #INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${file} DESTINATION ${directory})
ENDFOREACH(file)

# Define the executable target.

ADD_EXECUTABLE(yamss ${YAMSS_SOURCES} ${THRIFT_SOURCES})
TARGET_LINK_LIBRARIES(yamss ${EXTRA_LIBS})
INSTALL(TARGETS yamss RUNTIME DESTINATION bin)
