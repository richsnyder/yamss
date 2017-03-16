# List all of the core source and header files.

SET(SOURCES
    about.cpp
    element.cpp
    handler.cpp
    ostream.cpp
    this_handler.cpp
    transporter.cpp
)
SET(HEADERS
    yamss/about.hpp
    yamss/complex.hpp
    yamss/element.hpp
    yamss/eom.hpp
    yamss/handler.hpp
    yamss/input_reader.hpp
    yamss/iterate.hpp
    yamss/load.hpp
    yamss/map_values.hpp
    yamss/matrix_cast.hpp
    yamss/node.hpp
    yamss/run_simulation.hpp
    yamss/runner.hpp
    yamss/structure.hpp
    yamss/transporter.hpp
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
      INTERFACE_DEFINITION yamss/server/yamss.hid
      SOURCE_DIRECTORY include/yamss/server
      GENERATED_SOURCES HERMES_SOURCES
      GENERATED_HEADERS HERMES_HEADERS
  )
  LIST(APPEND EXE_SOURCES server.cpp run_server.cpp)
  LIST(APPEND HEADERS yamss/server/run_server.hpp yamss/server/server.hpp)
ENDIF(BUILD_SERVER)
INSTALL(FILES yamss/server/yamss.hid DESTINATION share/yamss)

# Identify the directories that contain include files.

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/include)

# Set options for the build.

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(FEATURES cxx_auto_type cxx_defaulted_functions)

# Define library targets.

ADD_LIBRARY(yamss-static STATIC ${SOURCES} ${HERMES_SOURCES})
TARGET_COMPILE_FEATURES(yamss-static PUBLIC ${FEATURES})
SET_TARGET_PROPERTIES(yamss-static PROPERTIES OUTPUT_NAME yamss)
SET_TARGET_PROPERTIES(yamss-static PROPERTIES MACOSX_RPATH ON)
INSTALL(TARGETS yamss-static ARCHIVE DESTINATION lib)

ADD_LIBRARY(yamss-shared SHARED ${SOURCES} ${HERMES_SOURCES})
TARGET_COMPILE_FEATURES(yamss-shared PUBLIC ${FEATURES})
TARGET_LINK_LIBRARIES(yamss-shared ${EXTRA_LIBS})
SET_TARGET_PROPERTIES(yamss-shared PROPERTIES OUTPUT_NAME yamss)
SET_TARGET_PROPERTIES(yamss-shared PROPERTIES MACOSX_RPATH ON)
INSTALL(TARGETS yamss-shared LIBRARY DESTINATION lib)

# Define the executable target.

ADD_EXECUTABLE(yamss ${EXE_SOURCES})
TARGET_COMPILE_FEATURES(yamss PUBLIC ${FEATURES})
TARGET_LINK_LIBRARIES(yamss yamss-shared)
INSTALL(TARGETS yamss RUNTIME DESTINATION bin)

# Define the Java wrapper target if that is enabled.

IF(BUILD_WRAPPER)
  INCLUDE(UseJava)
  INCLUDE(UseSWIG)

  LIST(APPEND HEADERS yamss/wrapper/wrapper.hpp)
  SET(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/java)
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/classes)

  SET_PROPERTY(SOURCE yamss/wrapper/yamss.i
               PROPERTY SWIG_FLAGS -package mstc.eng.jni.yamss)
  SET_PROPERTY(SOURCE yamss/wrapper/yamss.i PROPERTY CPLUSPLUS ON)
  SWIG_ADD_MODULE(yamss-jni java yamss/wrapper/yamss.i)
  SWIG_LINK_LIBRARIES(yamss-jni yamss-shared ${EXTRA_LIBS})
  TARGET_SOURCES(yamss-shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/wrapper.cpp)
  TARGET_SOURCES(yamss-shared PUBLIC ${swig_generated_file_fullname})
  SET_TARGET_PROPERTIES(yamss-jni PROPERTIES OUTPUT_NAME yamss)
  INSTALL(TARGETS yamss-jni LIBRARY DESTINATION lib)

  ADD_CUSTOM_COMMAND(TARGET yamss-jni POST_BUILD
      COMMAND ${Java_JAVAC_EXECUTABLE} -d classes java/*.java
      COMMAND ${Java_JAR_EXECUTABLE} -cfM yamss.jar -C classes .
  )
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/yamss.jar DESTINATION lib)
ENDIF(BUILD_WRAPPER)
INSTALL(FILES yamss/wrapper/yamss.i DESTINATION share/yamss)

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
