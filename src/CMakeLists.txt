#This CMake-file simply collects the sources and headers of the cg engine...

#get the source-and header files
set(CGE_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cgengine/source")
set(CGE_HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cgengine/headers")

set(cgengine_headers	
                        ${CGE_HEADER_PATH}/mesh/CGEMesh.h
                        ${CGE_HEADER_PATH}/object/CGEObject.h
                        ${CGE_HEADER_PATH}/object/CGEDrawableObject.h
                        ${CGE_HEADER_PATH}/math/CGEGeometry.h
                        ${CGE_HEADER_PATH}/textures/CGEQGLTextures.h
                        ${CGE_HEADER_PATH}/textures/CGETexture2D.h
                        ${CGE_HEADER_PATH}/math/CGETransformation.h
                        PARENT_SCOPE
)
set(cgengine_source     
                        ${CGE_SOURCE_PATH}/object/CGEObject.cpp
                        ${CGE_SOURCE_PATH}/mesh/CGEMesh.cpp
                        ${CGE_SOURCE_PATH}/textures/CGETexture2D.cpp
                        ${CGE_SOURCE_PATH}/math/CGETransformation.cpp
                        PARENT_SCOPE
)

set(CGE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/cgengine/headers" PARENT_SCOPE)
