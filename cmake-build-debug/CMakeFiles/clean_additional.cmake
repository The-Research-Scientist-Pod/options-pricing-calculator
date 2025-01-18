# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "src/gui/CMakeFiles/options_pricer_gui_autogen.dir/AutogenUsed.txt"
  "src/gui/CMakeFiles/options_pricer_gui_autogen.dir/ParseCache.txt"
  "src/gui/options_pricer_gui_autogen"
  )
endif()
