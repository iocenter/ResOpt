#include "modelitemmidpipe.h"

#include "midpipe.h"

namespace ResOptGui
{

ModelItemMidPipe::ModelItemMidPipe(MidPipe *pipe, const QString &file_name, QGraphicsItem *parent)
    : ModelItem(file_name, parent),
      p_midpipe(pipe)

{

    setScale(0.8);

    setToolTip("Pipe: " + QString::number(p_midpipe->number()));

}

} // namespace
