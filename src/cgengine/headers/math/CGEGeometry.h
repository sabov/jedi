/*****************************************************************

  Typedefinitions and implementations for geometrical objects

*****************************************************************/

#ifndef CGEGEOMETRY_H
#define CGEGEOMETRY_H

#ifndef __GLEW_H__
    #include <GL/glew.h>
#endif
#define GLM_FORCE_INLINE
//#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace CGEngine {

/*
  Typedefs for glm
*/
typedef glm::mat2 Matrix2x2 ;
typedef glm::mat3 Matrix3x3 ;
typedef glm::mat4 Matrix4x4 ;
typedef glm::vec4 Vec4      ;
typedef glm::vec3 Vec3      ;
typedef glm::vec2 Vec2      ;

typedef std::vector<Vec3>   Vec3DVec    ;
typedef std::vector<Vec2>   Vec2DVec    ;

/*
    Base Types for Points
    =====================
*/

template<class Scalar>
class CPoint2D
{
public:
    Scalar x;
    Scalar y;

    CPoint2D() {}

    CPoint2D(const Scalar& _x, const Scalar& _y)
    {
        x = _x; y = _y;
    }

    operator Vec2()
    {
        return Vec2(static_cast<float>(x), static_cast<float>(y));
    }
};

template<class Scalar>
class CPoint3D
{
public:
    Scalar x;
    Scalar y;
    Scalar z;

    CPoint3D() {}

    CPoint3D(const Scalar& _x, const Scalar& _y, const Scalar& _z)
    {
        x = _x; y = _y; z = _z;
    }

    operator Vec3()
    {
        return Vec3(static_cast<float>(x), static_cast<float>(y),  static_cast<float>(z));
    }
};

template<class Scalar>
class CPoint4D
{
public:
    Scalar x;
    Scalar y;
    Scalar z;
    Scalar w;

    CPoint4D() {}

    CPoint4D(const Scalar& _x, const Scalar& _y, const Scalar& _z, const Scalar& _w)
    {
        x = _x; y = _y; z = _z; w = _w;
    }

    operator Vec4()
    {
        return Vec4(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z),  static_cast<float>(w));
    }
};

//Type-Definitions
typedef CPoint2D<GLfloat>   GLPoint2f   ;
typedef CPoint2D<GLdouble>  GLPoint2d   ;
typedef CPoint3D<GLfloat>   GLPoint3f   ;
typedef CPoint3D<GLdouble>  GLPoint3d   ;
typedef CPoint4D<GLfloat>   GLPoint4f   ;
typedef CPoint4D<GLdouble>  GLPoint4d   ;

/*
  Base Type for Mesh-Vertices
  ===========================
*/

template<class PointVectorType, class TypeTexCoordinate>
class CVertex
{
public:
    PointVectorType     m_Position;
    TypeTexCoordinate   m_TexCoord;
    PointVectorType     m_NormalVec;

    CVertex() {}

    CVertex(const PointVectorType& _position, const TypeTexCoordinate& _texCoord, const PointVectorType& _normal)
    {
        m_Position  = _position ;
        m_TexCoord  = _texCoord ;
        m_NormalVec = _normal   ;
    }
};

typedef CVertex<GLPoint3f, GLPoint2f>   GLVertex3f          ;
typedef CVertex<GLPoint3d, GLPoint2d>   GLVertex3d          ;
typedef std::vector<GLVertex3f>         GLVertex3fVector    ;
typedef std::vector<GLVertex3d>         GLVertex3dVector    ;

} //End Namespace

#endif // CGEGEOMETRY_H
