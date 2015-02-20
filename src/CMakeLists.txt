# List all of the core source and header files.

SET(YAMSS_SOURCES
    yamss.cpp
)
SET(YAMSS_HEADERS
    yamss/cout_q_inspector.hpp
    yamss/eom.hpp
    yamss/evaluator.hpp
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
    yamss/tecplot_mode_inspector.hpp
    yamss/tecplot_q_inspector.hpp
    yamss/xml_inspector.hpp
    yamss/xml_reader.hpp
)

# Identify the directories that contain include files.

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

# Define the executable target.

ADD_EXECUTABLE(yamss ${YAMSS_SOURCES})
TARGET_LINK_LIBRARIES(yamss ${EXTRA_LIBS})
INSTALL(TARGETS yamss RUNTIME DESTINATION bin)
