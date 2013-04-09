#ifndef INSPECTORWELLCONTROL_H
#define INSPECTORWELLCONTROL_H

#include <QWidget>

#include "wellcontrol.h"

using ResOpt::WellControl;

class QLineEdit;
class QLabel;
class QComboBox;

namespace ResOptGui
{

class InspectorWellControl : public QWidget
{
    Q_OBJECT
public:
    explicit InspectorWellControl(double time, double value, double max, double min, WellControl::contol_type type,  QWidget *parent = 0, bool header = false);

    double value();
    double max();
    double min();
    WellControl::contol_type type();

signals:

public slots:

private:

    QLineEdit *p_max;
    QLineEdit *p_min;
    QLineEdit *p_value;
    QLabel *p_time;
    QComboBox *p_type;


};

} // namespace
#endif // INSPECTORWELLCONTROL_H
