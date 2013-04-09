#include "plot.h"

#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>

namespace ResOptGui
{

Plot::Plot(QWidget *parent) :
    QWidget(parent),
    m_custom_plot(this),
    m_max(10),
    m_min(0)
{
    QGridLayout *layout = new QGridLayout;

    setLayout(layout);

    // setting up the plot
    layout->addWidget(&m_custom_plot, 0, 0, 1, 3);

    m_custom_plot.setTitle("Objective Value");

    m_custom_plot.addGraph();

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

    // setting up the clear button
    p_btn_clear = new QPushButton("Clear Plot", this);
    connect(p_btn_clear, SIGNAL(clicked()), this, SLOT(clearCases()));

    layout->addWidget(p_btn_clear, 1, 1);


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


    m_custom_plot.graph(0)->addData(m_cases.size(), c->objectiveValue());

    // x axis range
    if(m_cases.size() >= 5) m_custom_plot.xAxis->setRange(0, m_cases.size() + 1);

    // y axis range
    if(c->objectiveValue() < m_min) m_min = c->objectiveValue();
    if(c->objectiveValue() > m_max) m_max = c->objectiveValue();

    double padding = (m_max - m_min)*0.1;

    m_custom_plot.yAxis->setRange(m_min - padding, m_max + padding);

    // checking if the x-axis tick step must change
    if(m_cases.size() > 10)
    {
        int tick = m_cases.size() / 10;
        m_custom_plot.xAxis->setTickStep(tick);
    }


    m_custom_plot.replot();

}


//-----------------------------------------------------------------------------------------------
// Clears the cases from the plot
//-----------------------------------------------------------------------------------------------
void Plot::clearCases()
{
    for(int i = 0; i < m_custom_plot.graphCount(); ++i)
    {
        m_custom_plot.graph(i)->clearData();
    }

    for(int i = 0; i < m_cases.size(); ++i) delete m_cases.at(i);
    m_cases.resize(0);

    m_max = 10;
    m_min = 0;

    m_custom_plot.xAxis->setRange(0, 5);
    m_custom_plot.yAxis->setRange(0, 5);

    m_custom_plot.replot();
}

} // namespace
