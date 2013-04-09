#include "plot.h"

#include <QHBoxLayout>

namespace ResOptGui
{

Plot::Plot(QWidget *parent) :
    QWidget(parent),
    m_custom_plot(this),
    m_entries(1),
    m_max(10),
    m_min(0)
{
    QHBoxLayout *layout = new QHBoxLayout;

    setLayout(layout);

    layout->addWidget(&m_custom_plot);

    m_custom_plot.setTitle("Objective Value");

    m_custom_plot.addGraph();

    //graph(0)->addData(0,1);
    //graph(0)->addData(1,2);

    m_custom_plot.graph(0)->setLineStyle(QCPGraph::lsNone);
    m_custom_plot.graph(0)->setScatterStyle(QCP::ssDisc);

    m_custom_plot.xAxis->setLabel("Model Evaluation #");
    m_custom_plot.xAxis->setRange(0, 5);
    m_custom_plot.xAxis->setAutoTickStep(false);
    m_custom_plot.xAxis->setTickStep(1.0);

    m_custom_plot.yAxis->setLabel("Objective Value");
    m_custom_plot.yAxis->setNumberFormat("gbc");

    m_custom_plot.setRangeDrag(Qt::Vertical);
    m_custom_plot.setRangeZoom(Qt::Vertical);

}

Plot::~Plot()
{
    for(int i = 0; i < m_cases.size(); ++i)
    {
        delete m_cases.at(i);
    }
    m_cases.resize(0);
}

//-----------------------------------------------------------------------------------------------
// Adds data from the case to the plot
//-----------------------------------------------------------------------------------------------
void Plot::addCase(Case *c)
{
    // adding the case to the vector
    m_cases.push_back(new Case(*c));


    m_custom_plot.graph(0)->addData(m_entries, c->objectiveValue());

    // x axis range
    if(m_entries >= 5) m_custom_plot.xAxis->setRange(0, m_entries + 1);

    // y axis range
    if(c->objectiveValue() < m_min) m_min = c->objectiveValue();
    if(c->objectiveValue() > m_max) m_max = c->objectiveValue();

    double padding = (m_max - m_min)*0.1;

    m_custom_plot.yAxis->setRange(m_min - padding, m_max + padding);

    // checking if the x-axis tick step must change
    if(m_entries > 10)
    {
        int tick = m_entries / 10;
        m_custom_plot.xAxis->setTickStep(tick);
    }

    m_entries++;

    m_custom_plot.replot();

}


} // namespace
