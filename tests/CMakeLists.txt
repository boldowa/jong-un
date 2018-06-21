#-------------------------------------------------
# UNKO test
#-------------------------------------------------

set(CPPUTEST_LIB CppUTest)

# Compile flags
if(MSVC)
	set(UNKO_TEST_FLAGS "")
else(MSVC)
	set(UNKO_TEST_FLAGS "")
	# encode flags
	if("Windows" STREQUAL "${CMAKE_SYSTEM_NAME}")
	add_flags(UNKO_TEST_FLAGS "-fexec-charset=cp932")
	endif("Windows" STREQUAL "${CMAKE_SYSTEM_NAME}")
	# Warning flags
	add_flags(UNKO_TEST_FLAGS "-Wall -std=c++11 -Wpedantic")
	if(UNKO_EXECUTE_TEST)
		add_flags(UNKO_TEST_FLAGS "-include CppUTest/MemoryLeakDetectorNewMacros.h")
		add_flags(UNKO_TEST_FLAGS "-include CppUTest/MemoryLeakDetectorMallocMacros.h")
	endif(UNKO_EXECUTE_TEST)
endif(MSVC)

if(EXTENSIONS)
	list(APPEND CPPUTEST_LIB CppUTestExt)
endif(EXTENSIONS)

file(GLOB_RECURSE TESTSOURCES
	"."  "*.cpp"
)

add_executable(unko_test ${TESTSOURCES})
set_target_properties(unko_test PROPERTIES
	COMPILE_FLAGS "${UNKO_TEST_FLAGS}"
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
	)
target_link_libraries(unko_test unko ${CPPUTEST_LIB})

#if(NOT MSVC)
#	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#		if(UNKO_EXECUTE_TEST)
#			add_flags(UNKO_TEST_FLAGS "-include CppUTest/MemoryLeakDetectorNewMacros.h")
#			add_flags(UNKO_TEST_FLAGS "-include CppUTest/MemoryLeakDetectorMallocMacros.h")
#		endif()
#	endif()
#endif(NOT MSVC)


