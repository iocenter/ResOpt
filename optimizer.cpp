#include "optimizer.h"
#include "runner.h"
#include <QEventLoop>

#include <iostream>

using std::cout;
using std::endl;

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
    // the finished() signal should be emitted when the optimizer has converged
    connect(this, SIGNAL(finished()), p_runner, SIGNAL(optimizationFinished()));
}

void Optimizer::runCases(CaseQueue *cases)
{
    // creating an event loop that waits for all the cases to finish in the runner
    QEventLoop loop;

    // this makes the event loop wait for the casesFinished() signal to quit
    connect(p_runner, SIGNAL(casesFinished()), &loop, SLOT(quit()));

    // sending the cases to the runner
    p_runner->evaluate(cases);

    // waiting for the runner to finish
    loop.exec();
}

} // namespace ResOpt
