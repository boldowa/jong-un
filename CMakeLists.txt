#-------------------------------------------------
# UNKO CMakeLists
#-------------------------------------------------
cmake_minimum_required(VERSION 3.1)

if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
	message(FATAL_ERROR "This project can't build. Please build with it: https://github.com/boldowa/smw-utils")
endif("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")

project(unko C CXX)

GenerateCompilerH(UNKO include/compiler.h)

#execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/include/common)
#if("Windows" STREQUAL "${CMAKE_SYSTEM_NAME}")
#	add_custom_target(strres
#		SOURCES "${PROJECT_BINARY_DIR}/include/common/strres_jp.h")
#	add_custom_command(OUTPUT "include/common/strres_jp.h"
#		WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
#		COMMAND iconv -c -f utf-8 -t cp932 -o "include/common/strres_jp.h" "${PROJECT_SOURCE_DIR}/include/common/strres_jp.h.in")
#else("Windows" STREQUAL "${CMAKE_SYSTEM_NAME}")
#	configure_file("${PROJECT_SOURCE_DIR}/include/common/strres_jp.h.in" "include/common/strres_jp.h" COPYONLY)
#endif("Windows" STREQUAL "${CMAKE_SYSTEM_NAME}")

option(UNKO_EXECUTE_TEST "Execute test suite" ON)
option(UNKO_COVERAGE "Check code coverage" ON)
set(UNKO_COVERAGE_EXECUTE FALSE)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(UNKO_EXECUTE_TEST OFF)
endif(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
if(UNKO_COVERAGE AND UNKO_EXECUTE_TEST)
	find_program(GCOV gcov)
	if(NOT GCOV)
		message(WARNING "-- gcov not found...")
		set(UNKO_COVERAGE OFF)
	else(NOT GCOV)
		message(STATUS "gcov found: ${GCOV}")
	endif(NOT GCOV)
endif(UNKO_COVERAGE AND UNKO_EXECUTE_TEST)

include_directories(
	"${PROJECT_BINARY_DIR}/include"
	"${CMAKE_BINARY_DIR}/bolib2/"
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../cpputest/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../bolib2/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../jansson/include"
	)

link_directories(
        "${PROJECT_BINARY_DIR}"
)

add_subdirectory(src)

if(UNKO_EXECUTE_TEST)
	add_subdirectory(tests)
	#add_custom_target(prepare_test
	#	COMMAND ${CMAKE_COMMAND} -E echo "-- Prepareing test data..."
	#	COMMAND ${CMAKE_COMMAND} -E echo "   copy_directory \"${PROJECT_SOURCE_DIR}/testdata\" \"${CMAKE_BINARY_DIR}/testdata\""
	#	COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/testdata" "${CMAKE_BINARY_DIR}/testdata"
	#	COMMAND ${CMAKE_COMMAND} -E echo "   remove_directory \"${CMAKE_BINARY_DIR}/testdata/@out\""
	#	COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/testdata/@out"
	#	COMMAND ${CMAKE_COMMAND} -E echo "   make_directory \"${CMAKE_BINARY_DIR}/testdata/@out\""
	#	COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/testdata/@out"
	#	)
	#add_custom_target(check ALL
	#	COMMAND ${CMAKE_COMMAND} -E echo "+++++ Test starts... +++++"
	#	COMMAND ${CMAKE_COMMAND} -E echo "-- Executes bolib2_test ..."
	#	COMMAND $<TARGET_FILE:bolib2_test> -c
	#	COMMAND ${CMAKE_COMMAND} -E echo "-- Removing test data..."
	#	COMMAND ${CMAKE_COMMAND} -E echo "   remove_directory \"${CMAKE_BINARY_DIR}/testdata/@out\""
	#	COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/testdata/@out"
	#	COMMAND ${CMAKE_COMMAND} -E echo "+++++ done. +++++"
	#	)
	enable_testing()
	add_test(NAME unko COMMAND unko_test)
	#add_dependencies(check prepare_test)
	#add_dependencies(check bolib2_test)
	#add_dependencies(bolib2_exe check)

	find_program(GDB gdb)
	if(NOT GDB)
		message(WARNING "gdb isn't found.")
	else(NOT GDB)
		# debug
		#add_custom_target(debug
		#	COMMAND ${CMAKE_COMMAND} -E echo "-- Executes gdb..."
		#	COMMAND gdb $<TARGET_FILE:bolib2_test>
		#	)
		#add_dependencies(debug prepare_test)
		#add_dependencies(debug bolib2_test)
	endif(NOT GDB)

	# coverage
	if(UNKO_COVERAGE_EXECUTE)
		#add_custom_target(coverage ALL
		#	COMMAND rm -rf gcov
		#	COMMAND mkdir gcov
		#	#COMMAND find src/CMakeFiles/bolib2.dir/ -type f -name "*.gcno" -print0 | xargs -0 env LANG=C gcov
		#	COMMAND mv *.gcov gcov
		#	)
		#add_dependencies(coverage check)
		#add_dependencies(bolib2_exe coverage)
	endif(UNKO_COVERAGE_EXECUTE)
endif(UNKO_EXECUTE_TEST)


set(CMAKE_BUILD_FLAGS "${CMAKE_C_FLAGS}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_flags(CMAKE_BUILD_FLAGS ${CMAKE_C_FLAGS_DEBUG})
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_flags(CMAKE_BUILD_FLAGS ${CMAKE_C_FLAGS_RELEASE})
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")


