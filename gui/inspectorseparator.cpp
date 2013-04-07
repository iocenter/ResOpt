#include "inspectorseparator.h"
#include "inspectorvariable.h"
#include "plotstreams.h"

#include "separator.h"
#include "intvariable.h"
#include "realvariable.h"

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>


using ResOpt::IntVariable;
using ResOpt::RealVariable;

namespace ResOptGui
{

InspectorSeparator::InspectorSeparator(Separator *sep, QWidget *parent) :
    QWidget(parent),
    p_separator(sep),
    m_btn_close("Close", this),
    m_btn_ok("Ok", this),
    m_btn_plot("Plot", this)
{

    setAttribute(Qt::WA_DeleteOnClose);


    construct();

    show();
}


//-----------------------------------------------------------------------------------------------
// constructs the view
//-----------------------------------------------------------------------------------------------
void InspectorSeparator::construct()
{
    setWindowTitle("Separator #" + QString::number(p_separator->number()) + " Properties");

    QGridLayout *layout = new QGridLayout(this);

    setLayout(layout);

    // setting up the variables
    QGroupBox *box_var = new QGroupBox("Variables", this);
    box_var->setStyleSheet("QGroupBox{border:2px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");

    QVBoxLayout *layout_var = new QVBoxLayout(box_var);
    box_var->setLayout(layout_var);

    p_var_install = new InspectorVariable("Install Time:", p_separator->installTime()->value(), p_separator->installTime()->max(), p_separator->installTime()->min(), this, true);
    layout_var->addWidget(p_var_install);

    p_var_capacity = new InspectorVariable("Capacity:", p_separator->removeCapacity()->value(), p_separator->removeCapacity()->max(), p_separator->removeCapacity()->min(), this);
    layout_var->addWidget(p_var_capacity);

    p_var_removefrac = new InspectorVariable("Removal fraction:", p_separator->removeFraction()->value(), p_separator->removeFraction()->max(), p_separator->removeFraction()->min(), this);
    layout_var->addWidget(p_var_removefrac);

    layout->addWidget(box_var, 0, 0, 1, 3);


    // setting up the buttons
    layout->addWidget(&m_btn_ok, 1, 0);
    connect(&m_btn_ok, SIGNAL(clicked()), this, SLOT(saveAndClose()));

    layout->addWidget(&m_btn_plot, 1, 1);
    connect(&m_btn_plot, SIGNAL(clicked()), this, SLOT(openPlot()));

    layout->addWidget(&m_btn_close, 1, 2);
    connect(&m_btn_close, SIGNAL(clicked()), this, SLOT(close()));



}


//-----------------------------------------------------------------------------------------------
// Saves the current values to the model, and closes the window
//-----------------------------------------------------------------------------------------------
void InspectorSeparator::saveAndClose()
{
    // install time
    p_separator->installTime()->setValue(p_var_install->value());
    p_separator->installTime()->setMax(p_var_install->max());
    p_separator->installTime()->setMin(p_var_install->min());

    // capacity
    p_separator->removeCapacity()->setValue(p_var_capacity->value());
    p_separator->removeCapacity()->setMax(p_var_capacity->max());
    p_separator->removeCapacity()->setMin(p_var_capacity->min());

    // fraction
    p_separator->removeFraction()->setValue(p_var_removefrac->value());
    p_separator->removeFraction()->setMax(p_var_removefrac->max());
    p_separator->removeFraction()->setMin(p_var_removefrac->min());

    emit sendMsg("Saving variable values for Separator #" + QString::number(p_separator->number()) + " to model...");

    close();
}

//-----------------------------------------------------------------------------------------------
// Opens the streams plot for the separator
//-----------------------------------------------------------------------------------------------
void InspectorSeparator::openPlot()
{
    QString title = "Plots for Separator #" + QString::number(p_separator->number());

    PlotStreams *plt = new PlotStreams(title, p_separator->streams());
}



} // namespace
