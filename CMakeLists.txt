cmake_minimum_required(VERSION "3.7.1")

include(FetchContent)

project("battleship")


set(THREADS_PREFER_PTREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

FetchContent_Declare(asio
	GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
	GIT_TAG master
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
)
FetchContent_MakeAvailable(asio)

FetchContent_GetProperties(asio)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)


include_directories(src src/network)

add_executable("battleship" "src/main.cpp" "src/map.cpp" "src/renderer.cpp" "src/servergame.cpp" "src/clientgame.cpp")

target_link_libraries("battleship" 
	PRIVATE Threads::Threads
	PUBLIC ${CURSES_LIBRARIES}
	PUBLIC asio)
