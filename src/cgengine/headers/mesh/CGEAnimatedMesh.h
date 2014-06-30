/*******************************************************************************
 *
 * Class for Animations
 *
 *******************************************************************************/

#ifndef CGEANIMATEDMESH_H
#define CGEANIMATEDMESH_H

#include "CGEMesh.h"

namespace CGEngine {

//
// Typedefs and Classheaders
//
class   CAnimatedMesh   ;
typedef boost::shared_ptr<CAnimatedMesh>    AnimatedMeshPtr ;

/*
 * Class for animations
 */
class CAnimatedMesh : public CMesh
{
private:
    CAnimatedMesh(const CAnimatedMesh&) {}
protected:

public:
    CAnimatedMesh() : CMesh() {}
    ~CAnimatedMesh() {}

};


}   //Namespace

#endif // CGEANIMATEDMESH_H
