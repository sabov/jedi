#ifndef CGEFULLSCREENQUAD_H
#define CGEFULLSCREENQUAD_H

#ifndef MESH_H
    #include "CGEMesh.h"
#endif

namespace CGEngine {

class CFullScreenQuad;
typedef boost::shared_ptr<CFullScreenQuad>  FullScreenQuadPtr   ;

class CFullScreenQuad : public CMesh
{
private:
    CFullScreenQuad(const CFullScreenQuad&) {}
public:
    CFullScreenQuad() : CMesh() {}
    ~CFullScreenQuad() {}

    bool LoadMesh(const std::string &_filename, unsigned int _postprocessingFlags  = CGE_NO_POSTPROCESSING);
    virtual void VOnDraw();
    bool HasProperty(CGEMeshProperty _property) const;
};

}   //Namespace

#endif // CGEFULLSCREENQUAD_H
