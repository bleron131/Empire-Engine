# Empire Engine - standalone build of the engine-independent simulation core.
#
# Compiles the same sources that Unreal Build Tool compiles as the EmpireSimCore module,
# so simulation logic can be built and tested without the Unreal Editor (e.g. in CI).
#
#   cmake -S Tests/SimCore -B build/simcore -G Ninja
#   cmake --build build/simcore
#   ctest --test-dir build/simcore --output-on-failure

cmake_minimum_required(VERSION 3.20)
project(EmpireSimCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(EMPIRE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
option(EMPIRE_SANITIZE "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

set(EMPIRE_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Source/EmpireSimCore")

# All module sources except Unreal-only glue (module registration needs Unreal headers).
file(GLOB_RECURSE EMPIRE_CORE_SOURCES CONFIGURE_DEPENDS "${EMPIRE_MODULE_DIR}/Private/*.cpp")
list(FILTER EMPIRE_CORE_SOURCES EXCLUDE REGEX ".*/EmpireSimCoreModule\\.cpp$")

add_library(EmpireSimCore STATIC ${EMPIRE_CORE_SOURCES})
target_include_directories(EmpireSimCore PUBLIC "${EMPIRE_MODULE_DIR}/Public")

# Mirror Unreal's defaults (no exceptions, no RTTI) and its strict warning settings,
# so code that builds here does not break once compiled by Unreal Build Tool.
if(MSVC)
	target_compile_options(EmpireSimCore PUBLIC /W4 /permissive- /GR- /EHs-c- /D_HAS_EXCEPTIONS=0)
	if(EMPIRE_WARNINGS_AS_ERRORS)
		target_compile_options(EmpireSimCore PUBLIC /WX)
	endif()
else()
	target_compile_options(EmpireSimCore PUBLIC
		-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wnon-virtual-dtor
		-fno-exceptions -fno-rtti)
	if(EMPIRE_WARNINGS_AS_ERRORS)
		target_compile_options(EmpireSimCore PUBLIC -Werror)
	endif()
	if(EMPIRE_SANITIZE)
		target_compile_options(EmpireSimCore PUBLIC -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer)
		target_link_options(EmpireSimCore PUBLIC -fsanitize=address,undefined)
	endif()
endif()

enable_testing()

file(GLOB EMPIRE_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(EmpireSimCoreTests ${EMPIRE_TEST_SOURCES})
target_include_directories(EmpireSimCoreTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(EmpireSimCoreTests PRIVATE EmpireSimCore)

add_test(NAME EmpireSimCoreTests COMMAND EmpireSimCoreTests)
