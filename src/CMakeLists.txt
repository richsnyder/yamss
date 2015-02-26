# List all of the core source and header files.

SET(YAMSS_SOURCES
    about.cpp
    clp.cpp
    yamss.cpp
)
SET(YAMSS_HEADERS
    yamss/about.hpp
    yamss/clp.hpp
    yamss/cout_q_inspector.hpp
    yamss/eom.hpp
    yamss/evaluator.hpp
    yamss/generalized_alpha.hpp
    yamss/inspector.hpp
    yamss/integrator.hpp
    yamss/iterate.hpp
    yamss/load.hpp
    yamss/lua_evaluator.hpp
    yamss/matrix_cast.hpp
    yamss/newmark_beta.hpp
    yamss/node.hpp
    yamss/rapidxml.hpp
    yamss/rapidxml_print.hpp
    yamss/runner.hpp
    yamss/structure.hpp
    yamss/tecplot_modes_inspector.hpp
    yamss/tecplot_q_inspector.hpp
    yamss/xml_inspector.hpp
    yamss/xml_reader.hpp
)
SET(YAMSS_CONFIG_HEADERS
    yamss/version.hpp
)

# Identify the directories that contain include files.

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/include)
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

# Process header files.

FOREACH(file ${YAMSS_HEADERS})
    GET_FILENAME_COMPONENT(subdirectory ${file} DIRECTORY)
    INSTALL(FILES ${file} DESTINATION include/${subdirectory})
ENDFOREACH(file)
FOREACH(file ${YAMSS_CONFIG_HEADERS})
    GET_FILENAME_COMPONENT(subdirectory ${file} DIRECTORY)
    SET(src ${CMAKE_CURRENT_SOURCE_DIR}/${file}.in)
    SET(dst ${CMAKE_CURRENT_BINARY_DIR}/include/${file})
    CONFIGURE_FILE(${src} ${dst})
    INSTALL(FILES ${dst} DESTINATION include/${subdirectory})
ENDFOREACH(file)

# Define the executable target.

ADD_EXECUTABLE(yamss ${YAMSS_SOURCES})
TARGET_LINK_LIBRARIES(yamss ${EXTRA_LIBS})
INSTALL(TARGETS yamss RUNTIME DESTINATION bin)
