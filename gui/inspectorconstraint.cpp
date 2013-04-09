#include "inspectorconstraint.h"

#include <QtGui/QLabel>
#include <QtGui/QGridLayout>

namespace ResOptGui
{


InspectorConstraint::InspectorConstraint(double time, double value, double max, double min, QWidget *parent, bool header):
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    int row = 0;

    // creating header if specified
    if(header)
    {
        layout->addWidget(new QLabel("time", this), row, 0, Qt::AlignCenter);
        layout->addWidget(new QLabel("min", this), row, 1, Qt::AlignCenter);
        layout->addWidget(new QLabel("value", this), row, 2, Qt::AlignCenter);
        layout->addWidget(new QLabel("max", this), row, 3, Qt::AlignCenter);

        row++;
    }

    // setting up the components

    p_time = new QLabel(QString::number(time), this);
   // p_time->setFixedWidth(100);
    layout->addWidget(p_time, row, 0, Qt::AlignCenter);

    p_min = new QLabel(QString::number(min), this);
   // p_max->setFixedWidth(100);
    layout->addWidget(p_min, row, 1, Qt::AlignCenter);


    p_value = new QLabel(this);
    p_value->setTextFormat(Qt::RichText);

    if(value < min || value > max) p_value->setText("<b><font color = red>" + QString::number(value) + "</font></b>");
    else p_value->setText("<b>" + QString::number(value) + "</b>");

   // p_value->setFixedWidth(100);
    layout->addWidget(p_value, row, 2, Qt::AlignCenter);


    p_max = new QLabel(QString::number(max), this);
   // p_max->setFixedWidth(100);
    layout->addWidget(p_max, row, 3, Qt::AlignCenter);





}


} // namespace
