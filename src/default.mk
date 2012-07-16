#!/bin/make -f

# Copy this file, it is only meant to show you which variables you might be able to set
# cp default.mk local.mk

# Do not change this file, and if you do, do not commit your changes to SVN unless they are useful for others

# Run on PC (contrary to an embedded device for example)
RUNONPC=true

# You can define a path to a cross-compiler
COMPILER_PATH=

# You can define the prefix for the compiler, e.g. "bfin-linux-uclibc-" or "arm-elf-" (with the dash at the end)
COMPILER_PREFIX=

# If a third party library needs to be included that is not on the system path, you can add the include files
# as space-separated values in  ADDITIONAL_INCLUDE_PATHS and the library itself in ADDITIONAL_LIBRARY_PATHS
ADDITIONAL_INCLUDE_PATHS=
ADDITIONAL_LIBRARY_PATHS=
