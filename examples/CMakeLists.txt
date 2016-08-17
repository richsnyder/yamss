SET(EXAMPLES
  harmonic_oscillators/damped.xml
  harmonic_oscillators/driven.xml
  harmonic_oscillators/simple.xml
  plates/simply_supported.xml
)

FOREACH(file ${EXAMPLES})
  GET_FILENAME_COMPONENT(subdirectory ${file} DIRECTORY)
  INSTALL(FILES ${file} DESTINATION share/yamss/examples/${subdirectory})
ENDFOREACH()
