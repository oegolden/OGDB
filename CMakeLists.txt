cmake_minimum_required(VERSION 3.14)
project(OGDB VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module for downloading Google Test
include(FetchContent)

# Fetch Google Test
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Add library source files
set(LIB_SOURCES
    lib/node.cpp
    lib/property.cpp
    lib/relationship.cpp
    lib/bufferPool.cpp
)

# Add library headers
set(LIB_HEADERS
    lib/node.h
    lib/property.h
    lib/relationship.h
    lib/page.h
    lib/bufferPool.h
)

# Create a library from the source files
add_library(ogdb_lib ${LIB_SOURCES} ${LIB_HEADERS})

# Include directories for the library
target_include_directories(ogdb_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Add test executables
set(TEST_TARGETS test_node test_page)
foreach(TEST_TARGET ${TEST_TARGETS})
    add_executable(${TEST_TARGET} tests/${TEST_TARGET}.cpp)
    target_link_libraries(${TEST_TARGET}
        ogdb_lib
        gtest
        gtest_main
    )
endforeach()

# Include Google Test's CMake functions
include(GoogleTest)
foreach(TEST_TARGET ${TEST_TARGETS})
    gtest_discover_tests(${TEST_TARGET})
endforeach()
# Optional: Add custom target to run tests easily
add_custom_target(run_tests
    COMMAND test_node
    COMMAND test_page
    DEPENDS test_node test_page
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running tests..."
)
