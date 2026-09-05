#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "tbaricault::str" for configuration ""
set_property(TARGET tbaricault::str APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(tbaricault::str PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libtbaricault_str.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS tbaricault::str )
list(APPEND _IMPORT_CHECK_FILES_FOR_tbaricault::str "${_IMPORT_PREFIX}/lib/libtbaricault_str.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
