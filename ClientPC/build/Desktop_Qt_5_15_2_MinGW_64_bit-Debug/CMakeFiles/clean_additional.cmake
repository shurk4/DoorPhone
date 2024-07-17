# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ClientPC_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ClientPC_autogen.dir\\ParseCache.txt"
  "ClientPC_autogen"
  )
endif()
