#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "tbaricault::cli" for configuration ""
set_property(TARGET tbaricault::cli APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(tbaricault::cli PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libtbaricault_cli.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS tbaricault::cli )
list(APPEND _IMPORT_CHECK_FILES_FOR_tbaricault::cli "${_IMPORT_PREFIX}/lib/libtbaricault_cli.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
