cmake_minimum_required(VERSION 3.20)
project(StyxBot  VERSION 1.0 LANGUAGES CXX)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a .so)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)

add_executable(StyxBot
		Src/Main.CPP
		Src/LoggingSystem.CPP
		Src/NetworkRequest.CPP
		Src/TelegramBotAPI.CPP
		Src/StyxSQLite.CPP
		Src/EventHandlerCenter.CPP
)

target_include_directories(StyxBot PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${SQLite3_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/Include
)

set_target_properties(StyxBot PROPERTIES
  LINK_SEARCH_START_STATIC ON
  LINK_SEARCH_END_STATIC   OFF
)
target_link_options(StyxBot PRIVATE
  "-static-libgcc"
  "-static-libstdc++"
)

target_link_libraries(StyxBot PRIVATE
    ${CURL_LIBRARIES}
    ${SQLite3_LIBRARIES}
)