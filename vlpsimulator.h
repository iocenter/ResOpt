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

    VlpSimulator(const VlpSimulator &v);

    virtual ~VlpSimulator();


    virtual ReservoirSimulator* clone() const {return new VlpSimulator(*this);}


    virtual bool generateInputFiles(Model *m);
    virtual bool launchSimulator();
    virtual bool readOutput(Model *m);

};

} // namespace ResOpt

#endif // VLPSIMULATOR_H
