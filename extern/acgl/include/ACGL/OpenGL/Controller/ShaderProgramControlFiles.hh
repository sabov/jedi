/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>

namespace ACGL{
namespace OpenGL{

typedef ACGL::OpenGL::ShaderProgramCreator ShaderProgramControlFiles;
#warning "ShaderProgramControlFiles is deprecated, use ShaderProgramCreator instead (same interface)"

}
}
