/**************************************************************************

  Process-Manager Class (for managing processes, duuuuuh...)

**************************************************************************/

#include <list>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "GLGProcess.h"

#ifndef PROCESSMANAGER_H
    #define PROCESSMANAGER_H
#ifndef NULL
    #define NULL 0
#endif // NULL

namespace GameLogic {

class CProcessManager   ;
typedef boost::shared_ptr<CProcessManager>  ProcessManagerPtr   ;

class CProcessManager
{
public:

    typedef std::list<ProcessPointer> ProcessList ;

    void attachProcess(const ProcessPointer& pProcess) ;    // append process to list

    void attachProcess(const ProcessPointer& pProcess, const ProcessPointer& dependingProcess) ; // append process to list, with dependency
    void detachProcess(const ProcessPointer& pProcess) ;     // remove process from list

    bool hasProcesses() const ;

    bool hasProcess(const ProcessPointer& pProcess) ;

    void updateProcesses(const int elapsedTime) ;   // update all processes in list

    static ProcessManagerPtr getInstance()
    {
        if (!m_Instance)
            m_Instance = ProcessManagerPtr(new CProcessManager) ;

        return m_Instance ;
    }

    static void destroyInstance()
    {
        m_Instance.reset(); ;
    }

    virtual ~CProcessManager()
    {
        m_ProcessList.clear();
        this->destroyInstance();
    }

private:
    static ProcessManagerPtr m_Instance   ;               // single instance of manger

    ProcessList m_ProcessList ;

    CProcessManager() {}
    CProcessManager(const CProcessManager&) {}



};

}   //Namespace


#endif // PROCESSMANAGER_H
