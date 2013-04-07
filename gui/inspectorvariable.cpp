#include "inspectorvariable.h"

#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>

namespace ResOptGui
{


InspectorVariable::InspectorVariable(const QString &name, double value, double max, double min, QWidget *parent, bool header):
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    int row = 0;

    // creating header if specified
    if(header)
    {
        layout->addWidget(new QLabel("value", this), row, 1, Qt::AlignCenter);
        layout->addWidget(new QLabel("max", this), row, 2, Qt::AlignCenter);
        layout->addWidget(new QLabel("min", this), row, 3, Qt::AlignCenter);

        row++;
    }

    // setting up the components

    p_name = new QLabel(name, this);
    p_name->setFixedWidth(150);
    layout->addWidget(p_name, row, 0);

    p_value = new QLineEdit(QString::number(value), this);
    p_value->setFixedWidth(100);
    layout->addWidget(p_value, row, 1);

    p_max = new QLineEdit(QString::number(max), this);
    p_max->setFixedWidth(100);
    layout->addWidget(p_max, row, 2);

    p_min = new QLineEdit(QString::number(min), this);
    p_max->setFixedWidth(100);
    layout->addWidget(p_min, row, 3);

}

//-----------------------------------------------------------------------------------------------
// Returns the current value
//-----------------------------------------------------------------------------------------------
double InspectorVariable::value()
{
    return p_value->text().toDouble();
}

//-----------------------------------------------------------------------------------------------
// Returns the current max
//-----------------------------------------------------------------------------------------------
double InspectorVariable::max()
{
    return p_max->text().toDouble();
}

//-----------------------------------------------------------------------------------------------
// Returns the current min
//-----------------------------------------------------------------------------------------------
double InspectorVariable::min()
{
    return p_min->text().toDouble();
}


} // namespace
