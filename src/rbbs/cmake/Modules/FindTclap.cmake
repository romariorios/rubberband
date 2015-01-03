# Taken from openbookfs
# https://github.com/cheshirekow/openbookfs
# - Try to find Tclap
# Once done, this will define
#
#  Tclap_FOUND        - system has Tclap
#  Tclap_INCLUDE_DIR  - the Tclap include directory

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Tclap_PKGCONF tclap)

# Main include dir
find_path(Tclap_INCLUDE_DIR
  NAMES tclap/CmdLine.h
  HINTS ${Tclap_PKGCONF_INCLUDE_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(Tclap_PROCESS_INCLUDES Tclap_INCLUDE_DIR)
libfind_process(Tclap)

