#ifndef MODELITEMENDPIPE_H
#define MODELITEMENDPIPE_H

#include "modelitem.h"

namespace ResOpt
{
class EndPipe;
}

using ResOpt::EndPipe;



namespace ResOptGui
{

class ModelItemEndPipe : public ModelItem
{
private:
    EndPipe *p_endpipe;

public:

    ModelItemEndPipe(EndPipe *pipe, const QString &file_name = ":new/images/endpipe_svg", QGraphicsItem *parent = 0);


    EndPipe* endPipe() {return p_endpipe;}


};

} // namespace

#endif // MODELITEMENDPIPE_H
