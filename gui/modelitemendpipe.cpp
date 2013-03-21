#include "modelitemendpipe.h"

#include "endpipe.h"

namespace ResOptGui
{

ModelItemEndPipe::ModelItemEndPipe(EndPipe *pipe, const QString &file_name, QGraphicsItem *parent)
    : ModelItem(file_name, parent),
      p_endpipe(pipe)

{

    setScale(0.8);

    setToolTip("Pipe: " + QString::number(p_endpipe->number()));

}

} // namespace
