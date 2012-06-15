#include "casequeue.h"

namespace ResOpt
{

CaseQueue::CaseQueue()
    : m_next_i(0)
{
}

//-----------------------------------------------------------------------------------------------
// returns the next Case in the queue
//-----------------------------------------------------------------------------------------------
Case* CaseQueue::next()
{
    if(m_next_i < 0 || m_next_i >= size()) return 0;
    else return at(m_next_i++);

}


} // namespace ResOpt
