# OGDB Test Suite

This directory contains tests for the OGDB project.

## Building and Running Tests

### Prerequisites
- CMake 3.14 or higher
- A C++17 compatible compiler (MSVC, GCC, or Clang)

### Build Instructions

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Configure the project:
```bash
cmake ..
```

3. Build the project:
```bash
cmake --build .
```

### Running Tests

After building, you can run the tests:

**Option 1: Run directly**
```bash
./test_node
# On Windows: .\Debug\test_node.exe or .\Release\test_node.exe
```

**Option 2: Use CTest**
```bash
ctest
```

**Option 3: Use the custom target**
```bash
cmake --build . --target run_tests
```

### Test Coverage

Currently includes:
- **test_node.cpp**: Comprehensive tests for the Node class
  - Constructor tests
  - Getter/setter tests
  - Serialization tests
  - Edge case validation
  - Exception handling tests

### Adding New Tests

To add new test files:
1. Create your test file in the `tests/` directory
2. Add it to CMakeLists.txt following the pattern for test_node
3. Rebuild the project

## Notes

- Google Test is automatically downloaded and configured by CMake
- Tests use the Google Test framework
- All tests should be independent and not rely on execution order
