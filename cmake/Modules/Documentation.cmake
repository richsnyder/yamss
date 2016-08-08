# ==============================================================================
#
# Documentation module
#
# ------------------------------------------------------------------------------
#
# Functions for generating documentation from Markdown and LaTeX source.
#
# ------------------------------------------------------------------------------
#
# This module defines the following functions:
#   ADD_DOCUMENTATION()
#     - Create a `doc` target to generate documentation.
#   DOC_SUMMARY()
#     - Generate a report on the configuration of the documentation.
#
# ==============================================================================

INCLUDE(CMakeParseArguments)

# ------------------------------------------------------------------------------
#
# _DOC_INITIALIZE()
#
# Add options that allow the user to configure the generation of documentation.
#
# This function is called automatically when the CMake module is first included.
#
# Define a `BUILD_DOC` option that takes any one of the following values: `ON`,
# `OFF`, or `AUTO`.  Documentation will be generated if it is set to `ON`,
# unless one or more of the required packages cannot be found, in which case a
# fatal error is raised.  If it's value is `AUTO`, then documentation will be
# generated only if all of the required packages can be found.  Upon exiting
# this function, `DOC_BUILD` will be set to either `ON` or `OFF`, indicating
# whether or not to build the documentation.
#
# If the `ENABLE_HTML` option is set, then HTML documentation will be produced.
#
# If the `ENABLE_PDF` option is set, then PDF documentation will be generated.
#
# ------------------------------------------------------------------------------

FUNCTION(_DOC_INITIALIZE)
  IF(NOT _DOC_FLAG_INITIALIZED)
    SET(_DOC_FLAG_INITIALIZED ON PARENT_SCOPE)
    SET(BUILD_DOC "AUTO" CACHE STRING "Build the documentation")
    SET_PROPERTY(CACHE BUILD_DOC PROPERTY STRINGS AUTO ON OFF)
    OPTION(ENABLE_HTML "Create documentation in HTML format" ON)
    OPTION(ENABLE_PDF "Create documentation in PDF format" OFF)
    OPTION(ENABLE_DOCX "Create documentation in DOCX format" OFF)

    IF(BUILD_DOC STREQUAL OFF)
      SET(flag OFF)
    ELSEIF(BUILD_DOC STREQUAL ON)
      SET(flag ON)
      FIND_PACKAGE(ImageMagick COMPONENTS convert REQUIRED)
      FIND_PACKAGE(LATEX COMPONENTS PDFLATEX REQUIRED)
      FIND_PACKAGE(Pandoc REQUIRED)
    ELSE()
      FIND_PACKAGE(ImageMagick COMPONENTS convert)
      FIND_PACKAGE(LATEX COMPONENTS PDFLATEX)
      FIND_PACKAGE(Pandoc)
      IF(ImageMagick_FOUND AND PDFLATEX_COMPILER AND Pandoc_FOUND)
        SET(flag ON)
      ELSE()
        SET(flag OFF)
      ENDIF()
    ENDIF()
    SET(DOC_BUILD ${flag} PARENT_SCOPE)
  ENDIF(NOT _DOC_FLAG_INITIALIZED)
ENDFUNCTION(_DOC_INITIALIZE)

# ------------------------------------------------------------------------------
#
# _DOC_MESSAGE(KEY <key> VALUE <value>
#              [INDENT <indent>]
#              [WIDTH <width>])
#
# Print a message about the documentation generation.
#
# Print a status message of the following format:
#
#   <leading><key><padding>: <value>
#
# where <leading> is <indent> spaces and <padding> is spacing that puts the
# colon into column <width> + 1.
#
# ------------------------------------------------------------------------------

FUNCTION(_DOC_MESSAGE)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
    ""
    "INDENT;KEY;VALUE;WIDTH"
    ""
    ${ARGN})

  # Set the indentation and message width.
  IF(NOT A_INDENT)
    SET(A_INDENT 2)
  ENDIF()
  IF(NOT A_WIDTH)
    SET(A_WIDTH 30)
  ENDIF()

  STRING(LENGTH "${A_KEY}" keylen)
  MATH(EXPR padlen "${A_WIDTH}-${A_INDENT}-${keylen}")
  STRING(RANDOM LENGTH ${A_INDENT} ALPHABET " " leading)
  STRING(RANDOM LENGTH ${padlen} ALPHABET " " padding)

  MESSAGE(STATUS "${leading}${A_KEY}${padding}: ${A_VALUE}")
ENDFUNCTION(_DOC_MESSAGE)

# ------------------------------------------------------------------------------
#
# _DOC_PDF_IMAGES([VAR <var>] [FILES <files>])
#
# Create custom commands for generating PDF files from LaTex source.
#
# Each entry in the <files> list is expected to be a LaTeX file.  A custom
# command is created for each LaTeX file to produce a PDF file.  The PDF files
# will be located in `${CMAKE_CURRENT_BINARY_DIR}/pdf/images`, but are not
# marked for installation.  If <var> is defined, then it is set to a list of
# absolute paths to the PDF files that will be generated.
#
# ------------------------------------------------------------------------------

FUNCTION(_DOC_PDF_IMAGES)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
    ""
    "VAR"
    "FILES"
    ${ARGN})

  # Create a directory to store the generated documents.
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pdf/images)

  # Process each file in the supplied list of files.
  FOREACH(file ${A_FILES})
    GET_FILENAME_COMPONENT(name_we "${file}" NAME_WE)
    SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
    SET(dst "${CMAKE_CURRENT_BINARY_DIR}/pdf/images/${name_we}.pdf")
    LIST(APPEND dependencies ${dst})
    ADD_CUSTOM_COMMAND(OUTPUT ${dst}
        COMMAND ${PDFLATEX_COMPILER} ${src} > /dev/null 2>&1
        DEPENDS ${src}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pdf/images)
  ENDFOREACH(file)

  # Return a list of the dependencies.
  IF(DEFINED A_VAR)
    SET(${A_VAR} ${dependencies} PARENT_SCOPE)
  ENDIF()
ENDFUNCTION(_DOC_PDF_IMAGES)

# ------------------------------------------------------------------------------
#
# _DOC_PNG_IMAGES([VAR <var>] [FILES <files>])
#
# Create custom commands for generating PNG files from LaTeX source.
#
# Each entry in the <files> list is expected to be a LaTeX file.  A custom
# command is created for each LaTeX file to produce a PNG file.  The PNG file
# is created from a PDF file that was, in turn, generated from the LaTeX
# source.  This function, therefore, depends on the `_DOC_PDF_IMAGES` function
# having been called to create the needed LaTeX-to-PDF commands.  The PNG files
# will be located in `${CMAKE_CURRENT_BINARY_DIR}/html/images` and will be
# marked for installation into the installed to `doc/html/images`.  If <var> is
# defined, then it is set to a list of absolute paths to the PDF files that
# will be generated.
#
# ------------------------------------------------------------------------------

FUNCTION(_DOC_PNG_IMAGES)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
    ""
    "VAR"
    "FILES"
    ${ARGN})

  # Create a directory to store the generated documents.
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/images)

  # Set options for the conversion.
  SET(options -density 150 -quality 90)

  # Process each file in the supplied list of files.
  FOREACH(file ${A_FILES})
    GET_FILENAME_COMPONENT(name_we "${file}" NAME_WE)
    SET(src "${CMAKE_CURRENT_BINARY_DIR}/pdf/images/${name_we}.pdf")
    SET(dst "${CMAKE_CURRENT_BINARY_DIR}/html/images/${name_we}.png")
    LIST(APPEND dependencies ${dst})
    ADD_CUSTOM_COMMAND(OUTPUT ${dst}
        COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} ${options} ${src} ${dst}
        DEPENDS ${src}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/images)
    INSTALL(FILES ${dst} DESTINATION doc/html/images)
  ENDFOREACH(file)

  # Return a list of the dependencies.
  IF(DEFINED A_VAR)
    SET(${A_VAR} ${dependencies} PARENT_SCOPE)
  ENDIF()
ENDFUNCTION(_DOC_PNG_IMAGES)

# ------------------------------------------------------------------------------
#
# _DOC_CSS([VAR <var>] [FILES <files>])
#
# Create custom commands for copying CSS files into the HTML directory.
#
# Each entry in the <files> list is expected to be a CSS file.  A custom
# command is created for each file that will copy the stylesheet into the
# HTML output directory.
#
# ------------------------------------------------------------------------------

FUNCTION(_DOC_CSS)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
    ""
    "VAR"
    "FILES"
    ${ARGN})

  # Process each file in the supplied list of files.
  FOREACH(file ${A_FILES})
    GET_FILENAME_COMPONENT(name_we "${file}" NAME_WE)
    SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
    SET(dst "${CMAKE_CURRENT_BINARY_DIR}/html/css/${name_we}.css")
    LIST(APPEND dependencies ${dst})
    ADD_CUSTOM_COMMAND(OUTPUT ${dst}
        COMMAND ${CMAKE_COMMAND} -E copy ${src} ${dst}
        DEPENDS ${src})
    INSTALL(FILES ${dst} DESTINATION doc/html/css)
  ENDFOREACH(file)

  # Return a list of the dependencies.
  IF(DEFINED A_VAR)
    SET(${A_VAR} ${dependencies} PARENT_SCOPE)
  ENDIF()
ENDFUNCTION(_DOC_CSS)

# ------------------------------------------------------------------------------
#
# _DOC_HTML([VAR <var>] [FILES <files>] [IMAGES <images>] [CSS <css>])
#
# Create custom commands for generating HTML files from Markdown source.
#
# Each entry in the <files> list is expected to be a Markdown file.  A custom
# command is created for each Markdown file to convert it into an HTML file.
# The generated HTML document can contain links to files in the <images> list.
# It can also use cascading style sheets contained in the <css> list.  The HTML
# files will be located in `${CMAKE_CURRENT_BINARY_DIR}/html` and will be
# marked for installation into `doc/html`.  If <var> is defined, then it is set
# to a list of absolute paths to the HTML files that will be generated.
#
# ------------------------------------------------------------------------------

FUNCTION(_DOC_HTML)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
    ""
    "VAR"
    "BIBLIOGRAPHIES;CSS;FILES;IMAGES"
    ${ARGN})

  # Create a directory to store the generated documents.
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html)

  # Set options for the conversion.
  LIST(APPEND options --standalone --normalize --smart)
  LIST(APPEND options --to=html5 --default-image-extension=png)
  LIST(APPEND options --mathml)
  LIST(APPEND options --filter pandoc-citeproc)

  # Create a list of stylesheets on which the documents depend.
  FOREACH(file ${A_CSS})
    GET_FILENAME_COMPONENT(name_we "${file}" NAME_WE)
    SET(dep "${CMAKE_CURRENT_BINARY_DIR}/html/css/${name_we}.css")
    SET(options ${options} --css="${dep}")
    LIST(APPEND stylesheets ${dep})
  ENDFOREACH(file)

  # Create a list of bibliographies on which the documents depend.
  FOREACH(file ${A_BIBLIOGRAPHIES})
    SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
    SET(dst "${CMAKE_CURRENT_BINARY_DIR}/html/${file}")
    LIST(APPEND bibliographies ${dst})
    ADD_CUSTOM_COMMAND(OUTPUT ${dst}
        COMMAND ${CMAKE_COMMAND} -E copy ${src} ${dst}
        DEPENDS ${src})
  ENDFOREACH(file)

  # Create a list of images on which the documents depend.
  FOREACH(file ${A_IMAGES})
    GET_FILENAME_COMPONENT(name_we "${file}" NAME_WE)
    SET(dep "${CMAKE_CURRENT_BINARY_DIR}/html/images/${name_we}.png")
    LIST(APPEND images ${dep})
  ENDFOREACH(file)

  # Process each file in the supplied list of files.
  FOREACH(file ${A_FILES})
    GET_FILENAME_COMPONENT(name_we "${file}" NAME_WE)
    SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
    SET(dst "${CMAKE_CURRENT_BINARY_DIR}/html/${name_we}.html")
    LIST(APPEND dependencies ${dst})
    ADD_CUSTOM_COMMAND(OUTPUT ${dst}
        COMMAND ${Pandoc_EXECUTABLE} ${options} ${src} -o ${dst}
        DEPENDS ${src} ${bibliographies} ${images} ${stylesheets}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html)
    INSTALL(FILES ${dst} DESTINATION doc/html)
  ENDFOREACH(file)

  # Return a list of the dependencies.
  IF(DEFINED A_VAR)
    SET(${A_VAR} ${dependencies} ${images} ${stylesheets} PARENT_SCOPE)
  ENDIF()
ENDFUNCTION(_DOC_HTML)

# ------------------------------------------------------------------------------
#
# _DOC_PDF([VAR <var>] [FILES <files>] [IMAGES <images>])
#
# Create custom commands for generating PDF files from Markdown source.
#
# Each entry in the <files> list is expected to be a Markdown file.  A custom
# command is created for each Markdown file to convert it into a PDF file.  The
# generated PDF document may contain embedded PDF files in the <images> list.
# The PDF files will be located in `${CMAKE_CURRENT_BINARY_DIR}/pdf` and will
# be marked for installation into `doc/pdf`.  If <var> is defined, then it is
# set to a list of absolute paths to the HTML files that will be generated.
#
# ------------------------------------------------------------------------------

FUNCTION(_DOC_PDF)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
    ""
    "VAR"
    "BIBLIOGRAPHIES;FILES;IMAGES"
    ${ARGN})

  # Create a directory to store the generated documents.
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pdf)

  # Set options for the conversion.
  LIST(APPEND options --standalone --normalize --smart)
  LIST(APPEND options --default-image-extension=pdf)
  LIST(APPEND options --filter pandoc-citeproc)

  # Create a list of bibliographies on which the documents depend.
  FOREACH(file ${A_BIBLIOGRAPHIES})
    SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
    SET(dst "${CMAKE_CURRENT_BINARY_DIR}/pdf/${file}")
    LIST(APPEND bibliographies ${dst})
    ADD_CUSTOM_COMMAND(OUTPUT ${dst}
        COMMAND ${CMAKE_COMMAND} -E copy ${src} ${dst}
        DEPENDS ${src})
  ENDFOREACH(file)

  # Create a list of images on which the documents depend.
  FOREACH(file ${A_IMAGES})
    GET_FILENAME_COMPONENT(name_we "${file}" NAME_WE)
    SET(pdf "${CMAKE_CURRENT_BINARY_DIR}/pdf/images/${name_we}.pdf")
    LIST(APPEND images ${pdf})
  ENDFOREACH(file)

  # Process each file in the supplied list of files.
  FOREACH(file ${A_FILES})
    GET_FILENAME_COMPONENT(name_we "${file}" NAME_WE)
    SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
    SET(wrk "${CMAKE_CURRENT_BINARY_DIR}/pdf/${file}")
    SET(dst "${CMAKE_CURRENT_BINARY_DIR}/pdf/${name_we}.pdf")
    LIST(APPEND dependencies ${dst})
    ADD_CUSTOM_COMMAND(OUTPUT ${dst}
        COMMAND ${CMAKE_COMMAND} -E copy ${src} ${wrk}
        COMMAND ${Pandoc_EXECUTABLE} ${options} ${file} -o ${dst}
        DEPENDS ${src} ${bibliographies} ${images}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pdf)
    INSTALL(FILES ${dst} DESTINATION doc/pdf)
  ENDFOREACH(file)

  # Return a list of the dependencies.
  IF(DEFINED A_VAR)
    SET(${A_VAR} ${dependencies} ${images} PARENT_SCOPE)
  ENDIF()
ENDFUNCTION(_DOC_PDF)

# ------------------------------------------------------------------------------
#
# _DOC_DOCX([VAR <var>] [FILES <files>] [IMAGES <images>] [STYLE <docx>])
#
# Create custom commands for generating DOCX files from Markdown source.
#
# Each entry in the <files> list is expected to be a Markdown file.  A custom
# command is created for each Markdown file to convert it into a DOCX file.
# The generated DOCX document may contain embedded PNG files in the <images>
# list.  The DOCX files will be located in `${CMAKE_CURRENT_BINARY_DIR}/docx`
# and will be marked for installation into `doc/word`.  If <var> is defined,
# then it is set to a list of absolute paths to the DOCX files that will be
# generated.
#
# ------------------------------------------------------------------------------

FUNCTION(_DOC_DOCX)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
    ""
    "VAR;STYLE"
    "BIBLIOGRAPHIES;FILES;IMAGES"
    ${ARGN})

  # Create a directory to store the generated documents.
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/word)

  # Set options for the conversion.
  LIST(APPEND options --standalone --normalize --smart)
  LIST(APPEND options --default-image-extension=png)
  LIST(APPEND options --filter pandoc-citeproc)

  # Create a list of stylesheets on which the documents depend.
  IF (DEFINED A_STYLE)
    SET(dep "${CMAKE_CURRENT_SOURCE_DIR}/${A_STYLE}")
    SET(options ${options} --reference-docx="${dep}")
    SET(stylesheet ${dep})
  ENDIF()

  # Create a list of bibliographies on which the documents depend.
  FOREACH(file ${A_BIBLIOGRAPHIES})
    SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
    SET(dst "${CMAKE_CURRENT_BINARY_DIR}/word/${file}")
    LIST(APPEND bibliographies ${dst})
    ADD_CUSTOM_COMMAND(OUTPUT ${dst}
        COMMAND ${CMAKE_COMMAND} -E copy ${src} ${dst}
        DEPENDS ${src})
  ENDFOREACH(file)

  # Create a list of images on which the documents depend.
  FOREACH(file ${A_IMAGES})
    GET_FILENAME_COMPONENT(name_we "${file}" NAME_WE)
    SET(src "${CMAKE_CURRENT_BINARY_DIR}/html/images/${name_we}.png")
    SET(dst "${CMAKE_CURRENT_BINARY_DIR}/word/images/${name_we}.png")
    LIST(APPEND images ${dst})
    ADD_CUSTOM_COMMAND(OUTPUT ${dst}
        COMMAND ${CMAKE_COMMAND} -E copy ${src} ${dst}
        DEPENDS ${src})
  ENDFOREACH(file)

  # Process each file in the supplied list of files.
  FOREACH(file ${A_FILES})
    GET_FILENAME_COMPONENT(name_we "${file}" NAME_WE)
    SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
    SET(wrk "${CMAKE_CURRENT_BINARY_DIR}/word/${file}")
    SET(dst "${CMAKE_CURRENT_BINARY_DIR}/word/${name_we}.docx")
    LIST(APPEND dependencies ${dst})
    ADD_CUSTOM_COMMAND(OUTPUT ${dst}
        COMMAND ${CMAKE_COMMAND} -E copy ${src} ${wrk}
        COMMAND ${Pandoc_EXECUTABLE} ${options} ${file} -o ${dst}
        DEPENDS ${src} ${bibliographies} ${images} ${stylesheet}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/word)
    INSTALL(FILES ${dst} DESTINATION doc/word)
  ENDFOREACH(file)

  # Return a list of the dependencies.
  IF(DEFINED A_VAR)
    SET(${A_VAR} ${dependencies} ${images} PARENT_SCOPE)
  ENDIF()
ENDFUNCTION(_DOC_DOCX)

# ------------------------------------------------------------------------------
#
# DOC_SUMMARY([INDENT <indent>] [WIDTH <width>])
#
# Print a summary of the documentation options.
#
# Prints status messages that summarize the settings that will be used to
# generate documentation.  The messages are indented by <indent> spaces.
#
# ------------------------------------------------------------------------------

FUNCTION(DOC_SUMMARY)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
    ""
    "INDENT;WIDTH"
    ""
    ${ARGN})

  # Set default indentation and width if they are not set.
  IF(NOT A_INDENT)
    SET(A_INDENT 2)
  ENDIF()
  IF(NOT A_WIDTH)
    SET(A_WIDTH 30)
  ENDIF()

  # Report on whether or not documentation will be generated.
  IF(DOC_BUILD)
    SET(doc_indicator "yes")
  ELSE()
    SET(doc_indicator "no")
  ENDIF()
  _DOC_MESSAGE(
      KEY "Build documentation"
      VALUE ${doc_indicator}
      INDENT ${A_INDENT}
      WIDTH ${A_WIDTH})

  # Report on the types of documentation that will be generated.
  IF(DOC_BUILD)
    IF(ENABLE_HTML)
      SET(html_indicator "yes")
    ELSE()
      SET(html_indicator "no")
    ENDIF()
    IF(ENABLE_PDF)
      SET(pdf_indicator "yes")
    ELSE()
      SET(pdf_indicator "no")
    ENDIF()
    IF(ENABLE_DOCX)
      SET(docx_indicator "yes")
    ELSE()
      SET(docx_indicator "no")
    ENDIF()
    _DOC_MESSAGE(
        KEY "Create HTML documentation"
        VALUE ${html_indicator}
        INDENT ${A_INDENT}
        WIDTH ${A_WIDTH})
    _DOC_MESSAGE(
        KEY "Create PDF documentation"
        VALUE ${pdf_indicator}
        INDENT ${A_INDENT}
        WIDTH ${A_WIDTH})
    _DOC_MESSAGE(
        KEY "Create DOCX documentation"
        VALUE ${docx_indicator}
        INDENT ${A_INDENT}
        WIDTH ${A_WIDTH})
  ENDIF()
ENDFUNCTION(DOC_SUMMARY)

# ------------------------------------------------------------------------------
#
# ADD_DOCUMENTATION(FILES <files> [IMAGES <images>] [CSS <css>])
#
# Generate documentation from a set of Markdown files and LaTeX images.
#
# Add a `doc` target that generates documentation from a set of Markdown files.
# LaTeX and CSS files may also be included.  Documentation can be generated in
# any of the following formats, as determined by `ENABLE_xxx` flags:
#
# - HTML
# - PDF
#
# The `doc` target is added to the default build target and will run every
# time that the `make` command is executed.
#
# ------------------------------------------------------------------------------

FUNCTION(ADD_DOCUMENTATION)

  # Parse the arguments to the function.
  CMAKE_PARSE_ARGUMENTS(A
    ""
    "TARGET;REFERENCE_DOCX"
    "BIBLIOGRAPHIES;CSS;FILES;IMAGES"
    ${ARGN})

  IF(NOT DEFINED A_TARGET)
    SET(A_TARGET doc)
  ENDIF()

  IF(DOC_BUILD)
    IF(ENABLE_HTML OR ENABLE_PDF OR ENABLE_DOCX)
      _DOC_PDF_IMAGES(FILES ${A_IMAGES})
    ENDIF()
    IF(ENABLE_HTML OR ENABLE_DOCX)
      _DOC_PNG_IMAGES(FILES ${A_IMAGES})
    ENDIF()
    IF(ENABLE_HTML)
      _DOC_CSS(FILES ${A_CSS})
      _DOC_HTML(
          VAR html_dependencies
          BIBLIOGRAPHIES ${A_BIBLIOGRAPHIES}
          CSS ${A_CSS}
          FILES ${A_FILES}
          IMAGES ${A_IMAGES})
      SET(dependencies ${dependencies} ${html_dependencies})
    ENDIF()
    IF(ENABLE_PDF)
      _DOC_PDF(
          VAR pdf_dependencies
          BIBLIOGRAPHIES ${A_BIBLIOGRAPHIES}
          FILES ${A_FILES}
          IMAGES ${A_IMAGES})
      SET(dependencies ${dependencies} ${pdf_dependencies})
    ENDIF()
    IF(ENABLE_DOCX)
      _DOC_DOCX(
          VAR docx_dependencies
          BIBLIOGRAPHIES ${A_BIBLIOGRAPHIES}
          FILES ${A_FILES}
          IMAGES ${A_IMAGES}
          STYLE ${A_REFERENCE_DOCX})
      SET(dependencies ${dependencies} ${docx_dependencies})
    ENDIF()
  ENDIF()
  ADD_CUSTOM_TARGET(${A_TARGET} ALL DEPENDS ${dependencies})

ENDFUNCTION(ADD_DOCUMENTATION)

# ------------------------------------------------------------------------------

_DOC_INITIALIZE()
