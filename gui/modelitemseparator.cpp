#include "modelitemseparator.h"
#include "inspectorseparator.h"
#include "modelscene.h"

#include "separator.h"


namespace ResOptGui
{

ModelItemSeparator::ModelItemSeparator(Separator *sep, const QString &file_name, QGraphicsItem *parent)
    : ModelItem(file_name, parent),
      p_separator(sep)

{

    setScale(0.6);

    setToolTip("Separator: " + QString::number(p_separator->number()));

}


//-----------------------------------------------------------------------------------------------
// Open the inspector window if item is double clicked
//-----------------------------------------------------------------------------------------------
void ModelItemSeparator::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    InspectorSeparator *inspector = new InspectorSeparator(p_separator);

    ModelScene *m_scene = dynamic_cast<ModelScene*>(scene());
    connect(inspector, SIGNAL(sendMsg(QString)), m_scene, SIGNAL(sendMsg(QString)));

}

} // namespace
