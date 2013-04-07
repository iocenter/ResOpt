#ifndef MODELITEMCAPACITY_H
#define MODELITEMCAPACITY_H

#include "modelitem.h"

namespace ResOpt
{
class Capacity;
}

using ResOpt::Capacity;



namespace ResOptGui
{

class ModelItemCapacity : public ModelItem
{
private:
    Capacity *p_capacity;

public:

    ModelItemCapacity(Capacity *cap, const QString &file_name = ":new/images/cap_svg", QGraphicsItem *parent = 0);


    Capacity* capacity() {return p_capacity;}


};

} // namespace

#endif // MODELITEMCAPACITY_H
