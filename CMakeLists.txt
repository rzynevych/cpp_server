cmake_minimum_required(VERSION 3.0)

project(cpp_server)

set(SERVER_EXECUTABLE_NAME server)
set(CLIENT_EXECUTABLE_NAME client)

SET(CMAKE_CXX_FLAGS "-g -std=c++11")

find_package(Boost REQUIRED)

add_executable(${SERVER_EXECUTABLE_NAME}
	server/src/Room.hpp
	server/src/SClient.hpp      
	server/src/Server.cpp       
	server/src/Server.hpp       
	server/src/server_common.hpp
	server/src/main.cpp
)

add_executable(${CLIENT_EXECUTABLE_NAME}
	client/src/Client.cpp  
	client/src/Client.hpp  
	client/src/Writer.hpp
	client/src/client_common.hpp  
	client/src/main.cpp 
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(${SERVER_EXECUTABLE_NAME} pthread)
	target_link_libraries(${CLIENT_EXECUTABLE_NAME} pthread)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	include_directories("${Boost_INCLUDE_DIRS}")
	target_link_libraries(${SERVER_EXECUTABLE_NAME} wsock32 ws2_32)
	target_link_libraries(${CLIENT_EXECUTABLE_NAME} wsock32 ws2_32)
endif()
