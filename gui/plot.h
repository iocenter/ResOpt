#ifndef PLOT_H
#define PLOT_H

#include <QtGui/QWidget>
#include <QVector>
#include "qcustomplot.h"


#include "case.h"

using ResOpt::Case;

class QPushButton;

namespace ResOptGui
{

class Plot : public QWidget
{
    Q_OBJECT

private:
    double m_max;
    double m_min;

    QPushButton *p_btn_clear;

    QCustomPlot m_custom_plot;
    QVector<Case*> m_cases;


public:
    Plot(QWidget *parent = 0);
    ~Plot();



public slots:

    void addCase(Case *c);
    void clearCases();
};


} // namespace

#endif // PLOT_H
