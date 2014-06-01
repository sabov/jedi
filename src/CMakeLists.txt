#This CMake-file simply collects the sources and headers of the cg engine and so on...

#get the source-and header files
#####################################################################
#CG Engine
#####################################################################

set(CGE_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cgengine/source")
set(CGE_HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cgengine/headers")

set(GLG_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/game_logic/source")
set(GLG_HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/game_logic/headers")

set(cgengine_headers	${CGE_HEADER_PATH}/mesh/CGEMesh.h
                        ${CGE_HEADER_PATH}/object/CGEObject.h
                        ${CGE_HEADER_PATH}/object/CGEDrawableObject.h
                        ${CGE_HEADER_PATH}/math/CGEGeometry.h
                        ${CGE_HEADER_PATH}/textures/CGEQGLTextures.h
                        ${CGE_HEADER_PATH}/textures/CGETexture2D.h
                        ${CGE_HEADER_PATH}/math/CGETransformation.h
                        PARENT_SCOPE
)
set(cgengine_source     ${CGE_SOURCE_PATH}/object/CGEObject.cpp
                        ${CGE_SOURCE_PATH}/mesh/CGEMesh.cpp
                        ${CGE_SOURCE_PATH}/textures/CGETexture2D.cpp
                        ${CGE_SOURCE_PATH}/math/CGETransformation.cpp
                        PARENT_SCOPE
)

########################################################################
#Game Logic
########################################################################

set(glg_headers	${GLG_HEADER_PATH}/process_sys/GLGProcessManager.h
		${GLG_HEADER_PATH}/process_sys/GLGProcess.h
		${GLG_HEADER_PATH}/process_sys/GLGExampleProcesses.h
                ${GLG_HEADER_PATH}/event_sys/hashed_string.h
                ${GLG_HEADER_PATH}/event_sys/GLGBaseEvent.h
                ${GLG_HEADER_PATH}/event_sys/GLGEventListener.h
                ${GLG_HEADER_PATH}/event_sys/GLGEventManagerInterface.h
                ${GLG_HEADER_PATH}/event_sys/GLGEventManager.h
		PARENT_SCOPE
)
set(glg_source	${GLG_SOURCE_PATH}/process_sys/GLGProcessManager.cpp
                ${GLG_SOURCE_PATH}/event_sys/GLGEventManager.cpp
		PARENT_SCOPE
)

set(CGE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/cgengine/headers" PARENT_SCOPE)
set(GLG_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/game_logic/headers" PARENT_SCOPE)
