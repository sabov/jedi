/*******************************************************************************

  Base Interface for CGEngine-Objects

  ********************************************************************************/

#ifndef CGEOBJECT_H
#define CGEOBJECT_H

namespace CGEngine{

class ICGEObject
{
protected:
    static long     m_currentID ;   //This holds the last generated ID (initially zero)
    long            m_ID        ;   //This is the ID of the CGEObject
public:
    //Base Constructor generates ID for Object
    ICGEObject()
    {
        m_currentID++ ;
        m_ID = m_currentID;
    }
    //Virtual Destructor
    virtual ~ICGEObject() {}

    long getObjectID() const
    {
        return m_ID;
    }
};

} //End Namespace

#endif // CGEOBJECT_H
