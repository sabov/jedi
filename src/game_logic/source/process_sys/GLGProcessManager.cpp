/*****************************************************************************************

  ProcessManager - Implementierung

*****************************************************************************************/

#include "process_sys/GLGProcessManager.h"

namespace GameLogic {

ProcessManagerPtr CProcessManager::m_Instance   ;

void CProcessManager::attachProcess(const ProcessPointer &pProcess) {
    this->m_ProcessList.push_back(pProcess);

    if (pProcess->m_InitialUpdate == false)
        pProcess->VOnInitialize();
}

void CProcessManager::attachProcess(const ProcessPointer &pProcess, const ProcessPointer &dependingProcess) {
    this->m_ProcessList.push_back(pProcess);

    if (pProcess->m_InitialUpdate == false)
        pProcess->VOnInitialize();

    pProcess->SetNext(dependingProcess);
}

bool CProcessManager::hasProcesses() const {
    return !this->m_ProcessList.empty() ;
}

void CProcessManager::updateProcesses(const int elapsedTime) {

    ProcessList::iterator i = m_ProcessList.begin() ;
    ProcessList::iterator end = m_ProcessList.end() ;

    ProcessPointer pNext ;

    while (i != end) {  //iteriere von Anfang bis Ende der Liste

        ProcessPointer p = *i ;

        if (p->isDead()) {
            //Überprüfe auf Nachfolger-Prozess und füge ihn in Liste ein, wenn vorhanden
            pNext = p->getNext() ;

            if (pNext) {
                attachProcess(pNext);
            }
            //Toten Prozess aus Liste entfernen
            i = m_ProcessList.erase(i) ;
        }
        else if (p->isActive() && !p->isPaused()) {
            //Aktive Prozesse updaten
            p->VOnUpdate(elapsedTime);
            ++i ;
        }
        else
            ++i ;

    }
}

void CProcessManager::detachProcess(const ProcessPointer &pProcess) {

    ProcessList::iterator i = m_ProcessList.begin() ;
    ProcessList::iterator end = m_ProcessList.end() ;

    while (pProcess != *i && i != end) {
        ++i ;
    }

    if (i == end) return ; //Prozess wurde nicht in Liste gefunden
    else m_ProcessList.erase(i) ;
}

bool CProcessManager::hasProcess(const ProcessPointer &pProcess) {
    ProcessList::iterator i = m_ProcessList.begin() ;
    ProcessList::iterator end = m_ProcessList.end() ;

    while (pProcess != *i && i != end) {
        ++i ;
    }

    if (i == end) return false ; //Prozess wurde nicht in Liste gefunden
    else return true ;
}

}
