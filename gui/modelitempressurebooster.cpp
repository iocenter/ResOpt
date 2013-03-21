#include "modelitempressurebooster.h"

#include "pressurebooster.h"

namespace ResOptGui
{

ModelItemPressureBooster::ModelItemPressureBooster(PressureBooster *booster, const QString &file_name, QGraphicsItem *parent)
    : ModelItem(file_name, parent),
      p_booster(booster)

{

    setScale(0.8);

    setToolTip("Booster: " + QString::number(p_booster->number()));

}

} // namespace
