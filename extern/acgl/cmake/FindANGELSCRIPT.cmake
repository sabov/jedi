# Find the native angelscript headers
#
#  ANGELSCRIPT_INCLUDE_DIR  - where to find angelscriptenc.h, etc.
#  ANGELSCRIPT_LIBRARIES    - angelscript library
#  ANGELSCRIPT_FOUND        - True if angelscript found.

IF(ANGELSCRIPT_FOUND)
    # Already in cache, be silent
    SET(ANGELSCRIPT_FIND_QUIETLY TRUE)
ENDIF()

IF(BUILD_ANGELSCRIPT OR ACGL_GLOBAL_EXTERN_DIR_FOUND_FIRST_TIME OR BUILD_TYPE_CHANGED)
    UNSET(ANGELSCRIPT_INCLUDE_DIR CACHE)
    UNSET(ANGELSCRIPT_LIBRARIES CACHE)
ENDIF()

IF(BUILD_ANGELSCRIPT)
    SET(ANGELSCRIPT_INCLUDE_DIR "${ACGL_LOCAL_EXTERN_DIR}/angelscript/angelscript/include/")
    SET(ANGELSCRIPT_LIBRARIES AngelScript${COMPILE_POSTFIX})
ELSE()
    # Look for the header file.
    IF(ACGL_GLOBAL_EXTERN_DIR)
        FIND_PATH(ANGELSCRIPT_INCLUDE_DIR NAMES angelscript.h PATHS "${ACGL_GLOBAL_EXTERN_DIR}/angelscript/angelscript/include/" NO_DEFAULT_PATH)
    ENDIF()
    IF(NOT ANGELSCRIPT_INCLUDE_DIR)
        FIND_PATH(ANGELSCRIPT_INCLUDE_DIR NAMES angelscript.h)
    ENDIF()

    # Look for the library file.
    IF(ACGL_GLOBAL_EXTERN_DIR)
        FIND_LIBRARY(ANGELSCRIPT_LIBRARIES NAMES AngelScript${COMPILE_POSTFIX} PATHS "${ACGL_GLOBAL_EXTERN_DIR}/angelscript/lib" NO_DEFAULT_PATH)
    ENDIF()
    IF(NOT ANGELSCRIPT_LIBRARIES)
        FIND_LIBRARY(ANGELSCRIPT_LIBRARIES NAMES AngelScript${COMPILE_POSTFIX})
    ENDIF()
ENDIF()

# Copy the results to the output variables.
IF(ANGELSCRIPT_INCLUDE_DIR AND ANGELSCRIPT_LIBRARIES)
    SET(ANGELSCRIPT_FOUND TRUE CACHE INTERNAL "")
ELSE()
    SET(ANGELSCRIPT_FOUND FALSE CACHE INTERNAL "")
ENDIF()

# Report the results.
IF(NOT ANGELSCRIPT_FOUND)
    SET(ANGELSCRIPT_MESSAGE "angelscript was not found. Make sure ANGELSCRIPT_INCLUDE_DIR AND ANGELSCRIPT_LIBRARIES are set correctly.")
    IF(ANGELSCRIPT_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "${ANGELSCRIPT_MESSAGE}")
    ELSEIF(NOT ANGELSCRIPT_FIND_QUIETLY)
        MESSAGE(STATUS "${ANGELSCRIPT_MESSAGE}")
    ENDIF()
ELSEIF(NOT ANGELSCRIPT_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for angelscript - found")
ENDIF()

MESSAGE(STATUS "ANGELSCRIPT_INCLUDE_DIR:${ANGELSCRIPT_INCLUDE_DIR}")
MESSAGE(STATUS "ANGELSCRIPT_LIBRARIES:${ANGELSCRIPT_LIBRARIES}")

