cmake_minimum_required (VERSION 2.8)
project(LIBREADCONFIG)

option(BUILD_HDF5 "Build HDF5 bindings" off)
option(BUILD_DOCS "Build documentation" off)

include(CheckIncludeFiles)
#include(CheckPrototypeExists)
include(CheckLibraryExists)
include(CheckCSourceCompiles)

CHECK_INCLUDE_FILES(stdio.h HAVE_STDIO_H)
CHECK_INCLUDE_FILES(dlfcn.h HAVE_DLFCN_H)
CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(locale.h HAVE_LOCALE_H)
CHECK_INCLUDE_FILES(memory.h HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILES(sys/stat.h HAVE_STAT_H)
CHECK_INCLUDE_FILES(sys/types.h HAVE_TYPES_H)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/src/config.h
)

add_definitions(-Wall -std=c99 -DHAVE_CONFIG_H)

if(BUILD_HDF5)
  CHECK_LIBRARY_EXISTS(hdf5 H5open "" HDF5_LIB)
  if(HDF5_LIB)
    add_definitions(-DHAVE_HDF5_H)
  endif(HDF5_LIB)
endif(BUILD_HDF5)

add_subdirectory(src)



