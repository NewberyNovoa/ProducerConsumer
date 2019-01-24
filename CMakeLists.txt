cmake_minimum_required (VERSION 3.10.2)
find_package (Threads)

#project (Itemprocesor)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -lpthread")
#set (CMAKE_STATIC_LINKER_FLAGS "-pthread")

add_executable (
	ProducerConsumer
	main.cpp
)
target_link_libraries (ProducerConsumer ${CMAKE_THREAD_LIBS_INIT})
