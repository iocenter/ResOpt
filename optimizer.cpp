#include "optimizer.h"

namespace ResOpt
{


Optimizer::Optimizer(Runner *r)
    : p_runner(r),
      m_initialized(false)
{
}

} // namespace ResOpt
