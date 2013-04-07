#ifndef INSPECTORSEPARATOR_H
#define INSPECTORSEPARATOR_H

#include <QWidget>
#include <QPushButton>

namespace ResOpt
{
class Separator;
}

using ResOpt::Separator;

namespace ResOptGui
{

class InspectorVariable;

class InspectorSeparator : public QWidget
{
    Q_OBJECT
private:
    Separator *p_separator;

    InspectorVariable *p_var_install;
    InspectorVariable *p_var_capacity;
    InspectorVariable *p_var_removefrac;

    QPushButton m_btn_close;
    QPushButton m_btn_ok;
    QPushButton m_btn_plot;

    void construct();


public:
    explicit InspectorSeparator(Separator *sep, QWidget *parent = 0);
    
signals:
    void sendMsg(QString);
    
public slots:
    void saveAndClose();
    void openPlot();
    
};

} // namespace

#endif // INSPECTORSEPARATOR_H
