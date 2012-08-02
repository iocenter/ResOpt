#ifndef VLPSIMULATOR_H
#define VLPSIMULATOR_H

#include "reservoirsimulator.h"

#include <QList>
#include <QStringList>

namespace ResOpt
{

class VlpTable;

class VlpSimulator : public ReservoirSimulator
{
private:

    QList<VlpTable*> m_vlp_tables;

    VlpTable* readVlpTable(const QString &file, const QString &well_name);
    QStringList processLine(const QString &line);
    bool isEmpty(const QStringList &list);

public:
    VlpSimulator();

    virtual ~VlpSimulator();

    virtual ReservoirSimulator* clone() const = 0;


    virtual bool generateInputFiles(Model *m);
    virtual bool launchSimulator() = 0;
    virtual bool readOutput(Model *m) = 0;

};

} // namespace ResOpt

#endif // VLPSIMULATOR_H
