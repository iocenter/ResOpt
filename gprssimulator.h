#ifndef GPRSSIMULATOR_H
#define GPRSSIMULATOR_H

#include <QStringList>
#include "reservoirsimulator.h"

class Well;

class GprsSimulator : public ReservoirSimulator
{
private:
    bool generateMainInputFile(Model *m);
    bool generateWellInputFile(Model *m);
    bool generateControlInputFile(Model *m);

    bool readWellOutput(Well *w, QString file_name);

    QStringList processLine(const QString &line);

public:
    GprsSimulator();

    // virtual functions

    virtual bool generateInputFiles(Model *m);
    virtual bool launchSimulator();
    virtual bool readOutput(Model *m);


};

#endif // GPRSSIMULATOR_H
