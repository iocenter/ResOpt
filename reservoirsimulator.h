#ifndef RESERVOIRSIMULATOR_H
#define RESERVOIRSIMULATOR_H

#include <QString>

class Model;

class ReservoirSimulator
{
private:
    QString m_folder;

public:
    ReservoirSimulator();


    virtual bool generateInputFiles(Model *m) = 0;
    virtual bool launchSimulator() = 0;
    virtual bool readOutput(Model *m) = 0;

    // set functions
    void setFolder(const QString &f) {m_folder = f;}

    // get functions
    const QString& folder() {return m_folder;}

};
#endif // RESERVOIRSIMULATOR_H
