#ifndef PLOTSTREAMZ_H
#define PLOTSTREAMZ_H

#include <QtGui/QWidget>
#include <QVector>

#include "qcustomplot.h"

#include "stream.h"

using ResOpt::Stream;

namespace ResOptGui
{

class PlotStreams : public QWidget
{
    Q_OBJECT

private:


    QCustomPlot m_custom_plot;

    QVector<Stream*> m_streams;

    void plotData();


public:
    PlotStreams(const QString &title, QVector<Stream*> streams, QWidget *parent = 0);



public slots:

};


} // namespace

#endif // PLOTSTREAMZ_H
