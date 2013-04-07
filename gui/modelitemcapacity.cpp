#include "modelitemcapacity.h"

#include "capacity.h"

namespace ResOptGui
{

ModelItemCapacity::ModelItemCapacity(Capacity *cap, const QString &file_name, QGraphicsItem *parent)
    : ModelItem(file_name, parent),
      p_capacity(cap)

{

    setScale(0.8);

    setToolTip("Capacity: " + p_capacity->name());

}

} // namespace
