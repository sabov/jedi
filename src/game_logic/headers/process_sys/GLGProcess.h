/**************************************************************************************

  Abstract Baseclass for Processes

**************************************************************************************/

#ifndef PROCESS_H
    #define PROCESS_H
#ifndef NULL
    #define NULL 0
#endif

#include <boost/smart_ptr/shared_ptr.hpp>

namespace GameLogic {

class CProcess        ;
class CProcessManager ;
typedef boost::shared_ptr<CProcess>   ProcessPointer  ;

class CProcess
{
    friend class CProcessManager ;

private:

protected:
    CProcess() {}

    bool            m_Kill ;         // process is dead/finished
    bool            m_Active ;       // process is active
    bool            m_Paused ;       // process is paused
    bool            m_InitialUpdate ;// process has been initialized

    ProcessPointer  m_pNext ;        // child process (will be executed when actual process has finished)

public:
    virtual ~CProcess() {}

    bool isDead() const {return m_Kill;}
    bool isActive() const {return m_Active;}
    bool isPaused() const {return m_Paused;}
    bool isInitialized() const {return m_InitialUpdate;}
    ProcessPointer getNext() const  {return m_pNext;}

    void SetActive(const bool b)  {m_Active = b;}
    void SetNext(const ProcessPointer& ppNext) {m_pNext = ppNext;}

    //Overloadables
    virtual void VOnUpdate(const int elapsedTime) = 0 ;
    virtual void VOnInitialize() = 0 ;
    virtual void VKill() = 0 ;
    virtual void VTogglePause() {m_Paused = ! m_Paused ;}
};

}   //Namespace

#endif // PROCESS_H
