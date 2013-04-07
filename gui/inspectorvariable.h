#ifndef INSPECTORVARIABLE_H
#define INSPECTORVARIABLE_H

#include <QWidget>

class QLineEdit;
class QLabel;

namespace ResOptGui
{

class InspectorVariable : public QWidget
{
    Q_OBJECT
public:
    explicit InspectorVariable(const QString &name, double value, double max, double min, QWidget *parent = 0, bool header = false);

    double value();
    double max();
    double min();
    
signals:
    
public slots:

private:

    QLineEdit *p_max;
    QLineEdit *p_min;
    QLineEdit *p_value;

    QLabel *p_name;
    QLabel *p_type;

    
};

} // namespace

#endif // INSPECTORVARIABLE_H
