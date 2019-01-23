add_library(cmsHandleTest cmsHandleTest.cpp)
# we need the FileCheck program in order to execute the check_clang_tidy.py script
if (LLVMFILECHECK)
else()
  message(STATUS "FileCheck not found; disabling the check_clang_tidy unit tests\n")
endif(LLVMFILECHECK)

