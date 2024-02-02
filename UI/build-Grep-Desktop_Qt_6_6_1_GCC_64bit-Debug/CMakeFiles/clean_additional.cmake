# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Grep_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Grep_autogen.dir/ParseCache.txt"
  "Grep_autogen"
  )
endif()
