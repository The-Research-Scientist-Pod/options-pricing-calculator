#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "options_pricer::options_pricer_lib" for configuration "Debug"
set_property(TARGET options_pricer::options_pricer_lib APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(options_pricer::options_pricer_lib PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/liboptions_pricer_lib.a"
  )

list(APPEND _cmake_import_check_targets options_pricer::options_pricer_lib )
list(APPEND _cmake_import_check_files_for_options_pricer::options_pricer_lib "${_IMPORT_PREFIX}/lib/liboptions_pricer_lib.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
