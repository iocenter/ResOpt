#ifndef CASEQUEUE_H
#define CASEQUEUE_H

#include <QVector>

namespace ResOpt
{

class Case;

class CaseQueue : public QVector<Case*>
{
private:
    int m_next_i;

public:
    CaseQueue();


    /**
     * @brief Returns the next Case in the queue
     * @details If there are no more cases in the queue, a null pointer is returned.
     *
     * @return Case
     */
    Case* next();
};

} // namespace ResOpt

#endif // CASEQUEUE_H
