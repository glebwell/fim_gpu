cmake_minimum_required(VERSION 2.8)
project(fim_gpu)
find_package(CUDA QUIET REQUIRED)
find_package(Threads)

# Pass options to NVCC
set( CUDA_PROPAGATE_HOST_FLAGS OFF )
set( CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -O3 -std=c++11)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

cuda_add_executable(
	frontier_expansion
	candidate_collection.cpp     
	candidate_collection.h
	frontier_node.cpp
	frontier_node.h
	mem_controller.cpp
	mem_controller.h
	frontier_preexpand.cpp
	frontier_preexpand.h
	cpu_interface.cpp           
	cpu_interface.h              
	global.cpp
	global.h
	gpu_interface.cu
	gpu_interface.h       
	data_interface.cpp                 
	data_interface.h                   
	frontier.cpp              
	frontier.h
	time_analysis.cpp
	time_analysis.h
	job_manager.cpp
	job_manager.h
        rule_generator.cpp
        rule_generator.h
	main.cpp
)

target_link_libraries (frontier_expansion ${CMAKE_THREAD_LIBS_INIT})
