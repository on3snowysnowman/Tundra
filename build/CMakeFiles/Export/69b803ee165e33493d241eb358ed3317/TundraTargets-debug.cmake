#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Tundra::Tundra" for configuration "Debug"
set_property(TARGET Tundra::Tundra APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Tundra::Tundra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libTundra.a"
  )

list(APPEND _cmake_import_check_targets Tundra::Tundra )
list(APPEND _cmake_import_check_files_for_Tundra::Tundra "${_IMPORT_PREFIX}/lib/libTundra.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
