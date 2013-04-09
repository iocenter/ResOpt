#ifndef INSPECTORPRODWELL_H
#define INSPECTORPRODWELL_H
#include <QWidget>
#include <QPushButton>
#include <QVector>

namespace ResOpt
{
class ProductionWell;
}

using ResOpt::ProductionWell;

namespace ResOptGui
{

class InspectorVariable;
class InspectorWellControl;
class InspectorConstraint;

class InspectorProdWell : public QWidget
{
    Q_OBJECT
private:
    ProductionWell *p_well;

    QVector<InspectorWellControl*> m_controls;
    QVector<InspectorConstraint*> m_bhp_constraints;


    QPushButton m_btn_close;
    QPushButton m_btn_ok;
    QPushButton m_btn_plot;

    void construct();


public:
    explicit InspectorProdWell(ProductionWell *well, QWidget *parent = 0);

signals:
    void sendMsg(QString);

public slots:
    void saveAndClose();
    void openPlot();

};

} // namespace


#endif // INSPECTORPRODWELL_H
