#include "optimizer.h"

namespace ResOpt
{


Optimizer::Optimizer(Runner *r)
    : p_runner(r),
      m_max_iter(1),
      m_perturbation_size(0.0001),
      m_initialized(false)
{
}

} // namespace ResOpt
