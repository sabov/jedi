/*********************************************************************************

  Geometrical Transformations

*********************************************************************************/

#ifndef CGETRANSFORMATION_H
#define CGETRANSFORMATION_H

#include <GL/glew.h>
#include <boost/shared_ptr.hpp>
#include <list>
#include "object/CGEObject.h"
#include "math/CGEGeometry.h"

namespace CGEngine {

//Class Header and Typedefs
class CMatrixStack ;
typedef boost::shared_ptr<CMatrixStack> MatrixStackPointer  ;
typedef std::list< Matrix4x4 >          MatrixStack         ;


class CMatrixStack : public ICGEObject
{
private :
    MatrixStack     m_Stack             ;
    Matrix4x4*      m_CompleteTransform ;

    CMatrixStack(const CMatrixStack&) {}
public:
    CMatrixStack() ;
    ~CMatrixStack() {m_Stack.clear();}

    //Translation
    void Translate(const Vec3& _TranslationVector)              ;
    //Rotation
    void Rotate(const float _Angle, const Vec3& _RotationAxis)  ;
    //Scaling
    void Scale(const Vec3& _ScalingFactors)                     ;
    void UniformScale(const float _ScalingFactor)               ;
    //Load Identity-Matrix to top position of the Stack
    void LoadIdentity()                                         ;
    //Push Stack
    void PushMatrix()                                           ;
    //Pop Stack
    void PopMatrix()                                            ;
    //Get the current transformation matrix (on top of the stack)
    const Matrix4x4& getCurrentTransform() const                ;
    //Get the complete transformation matrix (might be slow)
    Matrix4x4 getCompleteTransform() const                      ;
    //Get Pointer to current transformation-matrix (on top of the stack)
    const float* getCurrentTransformPointer() const             ;
    //Get Pointer to transformation-matrix
    const float* getCompleteTransformPointer()                  ;
    //(Re-)initializes the stack
    void EmptyStack()                                           ;
    //Compute Normalmatrix from given Modelmatrix and Viewmatrix
    static void ComputeNormalMatrix(Matrix3x3& _NormalMatrix,
                                    const Matrix4x4& _Model = Matrix4x4(1.0),
                                    const Matrix4x4& _View = Matrix4x4(1.0));

};

}   //End Namespace

#endif // CGETRANSFORMATION_H
