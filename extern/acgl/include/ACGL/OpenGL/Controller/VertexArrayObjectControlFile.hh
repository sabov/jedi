/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/OpenGL/Creator/VertexArrayObjectCreator.hh>

namespace ACGL{
namespace OpenGL{

typedef ACGL::OpenGL::VertexArrayObjectCreator VertexArrayObjectControlFile;
#warning "VertexArrayObjectControlFile is deprecated, use VertexArrayObjectCreator instead (same interface)"

}
}
