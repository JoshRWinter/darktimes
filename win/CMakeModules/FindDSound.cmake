# - Find DirectSound
# Find the DirectSound includes and libraries
#
#  DSOUND_INCLUDE_DIR - where to find dsound.h
#  DSOUND_LIBRARIES   - List of libraries when using dsound.
#  DSOUND_FOUND       - True if dsound found.

if(DSOUND_INCLUDE_DIR)
    # Already in cache, be silent
    set(DSOUND_FIND_QUIETLY TRUE)
endif(DSOUND_INCLUDE_DIR)

find_path(DSOUND_INCLUDE_DIR dsound.h
  "$ENV{DXSDK_DIR}"
  "$ENV{DXSDK_DIR}/Include"
)

find_library(DSOUND_LIBRARY dsound
  "$ENV{DXSDK_DIR}"
  "$ENV{DXSDK_DIR}/Lib"
  "$ENV{DXSDK_DIR}/Lib/x86"
)

# Handle the QUIETLY and REQUIRED arguments and set DSOUND_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DSound DEFAULT_MSG
    DSOUND_INCLUDE_DIR DSOUND_LIBRARY)

if(DSOUND_FOUND)
    set(DSOUND_LIBRARIES ${DSOUND_LIBRARY})
else(DSOUND_FOUND)
    set(DSOUND_LIBRARIES)
endif(DSOUND_FOUND)

mark_as_advanced(DSOUND_INCLUDE_DIR DSOUND_LIBRARY)
