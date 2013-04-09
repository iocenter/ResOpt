#include "modelitemprodwell.h"
#include "inspectorprodwell.h"
#include "modelscene.h"

#include "productionwell.h"

namespace ResOptGui
{

ModelItemProdWell::ModelItemProdWell(ProductionWell *prod, const QString &file_name, QGraphicsItem *parent)
    : ModelItem(file_name, parent),
      p_prod_well(prod)

{

    setScale(0.8);

    setToolTip("Production well: " + p_prod_well->name());

}

//-----------------------------------------------------------------------------------------------
// Open the inspector window if item is double clicked
//-----------------------------------------------------------------------------------------------
void ModelItemProdWell::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    InspectorProdWell *inspector = new InspectorProdWell(p_prod_well);

    ModelScene *m_scene = dynamic_cast<ModelScene*>(scene());
    connect(inspector, SIGNAL(sendMsg(QString)), m_scene, SIGNAL(sendMsg(QString)));

}


} // namespace
