#ifndef CASEQUEUE_H
#define CASEQUEUE_H

#include <QVector>

namespace ResOpt
{

class Case;

class CaseQueue : public QVector<Case*>
{
public:
    CaseQueue();
};

} // namespace ResOpt

#endif // CASEQUEUE_H
