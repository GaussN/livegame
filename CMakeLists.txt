cmake_minimum_required (VERSION 3.8)

project ("livegame")

add_executable (livegame "livegame.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET livegame PROPERTY CXX_STANDARD 20)
endif()