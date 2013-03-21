#ifndef MODELITEMMIDPIPE_H
#define MODELITEMMIDPIPE_H

#include "modelitem.h"

namespace ResOpt
{
class MidPipe;
}

using ResOpt::MidPipe;



namespace ResOptGui
{

class ModelItemMidPipe : public ModelItem
{
private:
    MidPipe *p_midpipe;

public:

    ModelItemMidPipe(MidPipe *pipe, const QString &file_name = ":new/images/midpipe_svg", QGraphicsItem *parent = 0);


    MidPipe* midPipe() {return p_midpipe;}


};

} // namespace

#endif // MODELITEMMIDPIPE_H
