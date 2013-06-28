#ifndef VLPSIMULATOR_H
#define VLPSIMULATOR_H

#include "reservoirsimulator.h"

#include <QList>
#include <QStringList>
class QFile;

namespace ResOpt
{

class VlpTable;


class VlpSimulator : public ReservoirSimulator
{
private:

    QList<VlpTable*> m_vlp_tables;

    bool readInput(const QString &file);
    VlpTable* readVlpTable(const QString &well_name, QFile &input);

    VlpTable* findVlpTable(const QString &well_name);

    QStringList processLine(const QString &line);
    bool isEmpty(const QStringList &list);

public:
    VlpSimulator();

    VlpSimulator(const VlpSimulator &v);

    virtual ~VlpSimulator();


    virtual ReservoirSimulator* clone() const {return new VlpSimulator(*this);}

    virtual QString description() const {return QString("SIMULATOR VLP\n\n");}

    virtual bool generateInputFiles(Model *m);
    virtual bool launchSimulator();
    virtual bool readOutput(Model *m);

    int numberOfVlpTables() const {return m_vlp_tables.size();}
    VlpTable* vlpTable(int i) {return m_vlp_tables.at(i);}

};

} // namespace ResOpt

#endif // VLPSIMULATOR_H
