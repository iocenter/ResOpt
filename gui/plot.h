#ifndef PLOT_H
#define PLOT_H

#include <QtGui/QWidget>
#include <QVector>
#include "qcustomplot.h"

#include "case.h"

using ResOpt::Case;

namespace ResOptGui
{

class Plot : public QWidget
{
    Q_OBJECT

private:
    int m_entries;
    double m_max;
    double m_min;

    QCustomPlot m_custom_plot;
    QVector<Case*> m_cases;


public:
    Plot(QWidget *parent = 0);
    ~Plot();



public slots:

    void addCase(Case *c);
};


} // namespace

#endif // PLOT_H
