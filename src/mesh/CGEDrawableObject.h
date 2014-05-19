/*******************************************************************************

    Interface for all Objects that can be passed to the
    rendering pipeline

********************************************************************************/

#ifndef DRAWABLEOBJECT_H
#define DRAWABLEOBJECT_H

#include "CGEObject.h"

namespace CGEngine {

class IDrawableObject : public ICGEObject {
public:
    virtual ~IDrawableObject() {}
    virtual void VOnDraw() = 0 ;

};

} //End Namespace

#endif // DRAWABLEOBJECT_H
