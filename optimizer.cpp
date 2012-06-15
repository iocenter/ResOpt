#include "optimizer.h"
#include "runner.h"
#include <QEventLoop>

namespace ResOpt
{


Optimizer::Optimizer(Runner *r)
    : QObject(r),
      p_runner(r),
      m_max_iter(1),
      m_parallel_runs(1),
      m_perturbation_size(0.0001),
      m_initialized(false)
{
    connect(this, SIGNAL(finished()), p_runner, SIGNAL(optimizationFinished()));
}

void Optimizer::requestCases(CaseQueue *cases)
{
    // creating an event loop that waits for all the cases to finish in the runner
    QEventLoop loop;
    loop.connect(p_runner, SIGNAL(casesFinished()), SLOT(quit()));

    // sending the cases to the runner
    p_runner->evaluate(cases);

    loop.exec();
}

} // namespace ResOpt
