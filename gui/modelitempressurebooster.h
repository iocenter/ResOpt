#ifndef MODELITEMPRESSUREBOOSTER_H
#define MODELITEMPRESSUREBOOSTER_H

#include "modelitem.h"

namespace ResOpt
{
class PressureBooster;
}

using ResOpt::PressureBooster;



namespace ResOptGui
{

class ModelItemPressureBooster : public ModelItem
{
private:
    PressureBooster *p_booster;

public:

    ModelItemPressureBooster(PressureBooster *booster, const QString &file_name = ":new/images/booster_svg", QGraphicsItem *parent = 0);


    PressureBooster* pressureBooster() {return p_booster;}


};

} // namespace

#endif // MODELITEMPRESSUREBOOSTER_H
