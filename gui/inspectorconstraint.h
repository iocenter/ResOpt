#ifndef INSPECTORCONSTRAINT_H
#define INSPECTORCONSTRAINT_H

#include <QWidget>

class QLabel;

namespace ResOptGui
{

class InspectorConstraint : public QWidget
{
    Q_OBJECT
public:
    explicit InspectorConstraint(double time, double value, double max, double min, QWidget *parent = 0, bool header = false);


signals:

public slots:

private:
    QLabel *p_time;
    QLabel *p_max;
    QLabel *p_min;
    QLabel *p_value;




};

} // namespace

#endif // INSPECTORCONSTRAINT_H
