# CMakeList.txt: проект CMake для livegame; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

project ("livegame")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (livegame "livegame.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET livegame PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
