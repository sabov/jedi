  ///////////////////////////////////////////////
 // Nur Beispiele...                          //
///////////////////////////////////////////////

#ifndef EXAMPLE_PROCESSES_H
#define EXAMPLE_PROCESSES_H
#ifndef NULL
#define NULL  0
#endif

#include <math.h>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "GLGProcess.h"

namespace GameLogic {

class RotationProcess;
typedef boost::shared_ptr<RotationProcess> RotationProcessPtr;

//Rotation für den Bunny steuern (kann aber alle möglichen Rotationen steuern)

//                          Von Prozessklasse abgeleitet
//                                  ||
//                                  ||
//                                  \/
class RotationProcess : public CProcess {
public:
    GLfloat rotAngle ;

    RotationProcess() {
        rotAngle = 0.0f ;
        this->m_Kill = false ;
        this->m_Active = true ;
        this->m_InitialUpdate = true ;
        this->m_Paused = false ;
        //this->m_pNext = NULL ;
    }
    //Geerbte Update-Funktion überladen (d.h. was soll bei update dieses Prozesses passiern?) ------|
    virtual void VOnUpdate(const int elapsedTime) {    //                                           |
        rotAngle += static_cast<float>(elapsedTime) / 25.0f ; //neuer Winkel wird berechnet <-------|

        if (rotAngle > 360.0) { //überschreitet der Winkel 360° soll Prozess beendet werden
            this->VKill();
        }
    }

    virtual void VKill() {
        this->m_Kill = true ;
        this->m_Active = false ;
        this->m_InitialUpdate = false ;
        //                    *******
    }   //                        |
        //                        |------> wird der Prozess nochmal in den ProzessManager geladen,
        //                                  sieht dieser, dass der Prozess neu initialisiert werden soll, dazu ---|
        //                                                                                                        |
    virtual void VOnInitialize() {//   <--------------------------------------------------------------------------|
        rotAngle = 0.0f ;
        this->m_Kill = false ;
        this->m_Active = true ;
        this->m_InitialUpdate = true ;
        this->m_Paused = false ;
    }

    virtual ~RotationProcess() {}

};


}   //Namespace

#endif // EXAMPLE_PROCESSES_H
