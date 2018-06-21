#-------------------------------------------------
# UNKO main sources
#-------------------------------------------------

# Compile flags
if(MSVC)
	set(UNKO_FLAGS "")
else(MSVC)
	set(UNKO_FLAGS "-fno-strict-aliasing")
	# encode flags
	if("Windows" STREQUAL "${CMAKE_SYSTEM_NAME}")
	add_flags(UNKO_FLAGS "-fexec-charset=cp932")
	endif("Windows" STREQUAL "${CMAKE_SYSTEM_NAME}")
	# Warning flags
	add_flags(UNKO_FLAGS "-Wall -Werror -Wpedantic -Wno-format -Wconversion -Wimplicit-function-declaration")
	# Test flags
	if(UNKO_EXECUTE_TEST)
		add_flags(UNKO_FLAGS "-include CppUTest/MemoryLeakDetectorNewMacros.h")
		add_flags(UNKO_FLAGS "-include CppUTest/MemoryLeakDetectorMallocMacros.h")
		add_flags(UNKO_FLAGS "-DCPPUTEST")
		if(UNKO_COVERAGE)
			add_flags(UNKO_FLAGS "--coverage")
		endif(UNKO_COVERAGE)
	endif(UNKO_EXECUTE_TEST)
endif(MSVC)



# ---------Build CUI Application

file(GLOB_RECURSE CUI_Sources
	"*.c"
        )
list(FILTER CUI_Sources EXCLUDE REGEX ".*main.c$")
if(MINGW OR MSVC)
	set(CUI_Resource "${CMAKE_CURRENT_SOURCE_DIR}/res/unko.rc")
endif(MINGW OR MSVC)
add_library(unko ${CUI_Sources} ${CUI_Resource})
#add_dependencies(unko strres)
set_target_properties(unko PROPERTIES
	COMPILE_FLAGS "${UNKO_FLAGS}"
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
	)
if(UNIX OR APPLE)
	target_link_libraries(unko dl asar)
	if(UNKO_EXECUTE_TEST)
		target_link_libraries(unko CppUTest)
		if(UNKO_COVERAGE)
			target_link_libraries(unko gcov)
		endif(UNKO_COVERAGE)
	endif(UNKO_EXECUTE_TEST)
endif(UNIX OR APPLE)
if(MSVC)
	set_target_properties(unko PROPERTIES LINK_FLAGS "/MANIFEST:NO ${WIN2000OBJ}")
endif(MSVC)
target_link_libraries(unko bolib2)

add_executable(unko_exe main.c)
target_link_libraries(unko_exe unko)
set_target_properties(unko_exe PROPERTIES OUTPUT_NAME unko)

