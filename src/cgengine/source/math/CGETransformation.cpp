#include "math/CGETransformation.h"

namespace CGEngine {

CMatrixStack::CMatrixStack()
{
    m_Stack.clear();
    m_Stack.push_back(Matrix4x4(1.0));
    m_CompleteTransform = NULL;
}

void CMatrixStack::Translate(const Vec3 &_TranslationVector)
{
    m_Stack.back() = Matrix4x4( glm::translate(m_Stack.back(), _TranslationVector) ) ;
}

void CMatrixStack::Rotate(const float _Angle, const Vec3 &_RotationAxis)
{
    m_Stack.back() = Matrix4x4( glm::rotate(m_Stack.back(), _Angle, _RotationAxis) ) ;
}

void CMatrixStack::Scale(const Vec3 &_ScalingFactors)
{
    m_Stack.back() = Matrix4x4( glm::scale(m_Stack.back(), _ScalingFactors) ) ;
}

void CMatrixStack::UniformScale(const float _ScalingFactor)
{
    Scale( Vec3(_ScalingFactor,_ScalingFactor,_ScalingFactor) );
}

void CMatrixStack::LoadIdentity()
{
    m_Stack.back() = Matrix4x4(1.0);
}

void CMatrixStack::PushMatrix()
{
    m_Stack.push_back( Matrix4x4(1.0) );
}

void CMatrixStack::PopMatrix()
{
    m_Stack.pop_back();
}

const Matrix4x4 &CMatrixStack::getCurrentTransform() const
{
    return m_Stack.back();
}

Matrix4x4 CMatrixStack::getCompleteTransform() const
{
    Matrix4x4 transfm = Matrix4x4(1.0);
    for ( MatrixStack::const_iterator m_it = m_Stack.begin(); m_it != m_Stack.end(); ++m_it )
    {
        transfm *= *m_it;
    }
    return transfm;
}

const float *CMatrixStack::getCurrentTransformPointer() const
{
    return glm::value_ptr( m_Stack.back() );
}

const float *CMatrixStack::getCompleteTransformPointer()
{
    if ( m_CompleteTransform )
        delete m_CompleteTransform;
    else
        m_CompleteTransform = new Matrix4x4(1.0);
    for ( MatrixStack::const_iterator m_it = m_Stack.begin(); m_it != m_Stack.end(); ++m_it )
    {
        *m_CompleteTransform *= *m_it;
    }
    return glm::value_ptr( *m_CompleteTransform );
}

void CMatrixStack::EmptyStack()
{
    m_Stack.clear();
    m_Stack.push_back( Matrix4x4(1.0) );
    if ( m_CompleteTransform )
        delete m_CompleteTransform;
    m_CompleteTransform = NULL;
}

void CMatrixStack::ComputeNormalMatrix(Matrix3x3 &_NormalMatrix, const Matrix4x4 &_Model, const Matrix4x4 &_View)
{
    Matrix4x4 modelview = _View * _Model;
    _NormalMatrix = glm::inverseTranspose( Matrix3x3( modelview ) );
}


}   //End Namespace
