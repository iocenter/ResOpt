#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>

namespace ResOpt
{

class Model;
class ReservoirSimulator;
class Case;

class Launcher : public QObject
{
    Q_OBJECT
private:
    Model *p_model;
    ReservoirSimulator *p_simulator;



public:
    explicit Launcher(QObject *parent = 0);

    bool initialize();

    // set functions
    void setModel(Model *m) {p_model = m;}
    void setReservoirSimulator(ReservoirSimulator *s) {p_simulator = s;}

    // get functions
    Model* model() {return p_model;}
    ReservoirSimulator* reservoirSimulator() {return p_simulator;}
    
signals:
    void finished(Launcher *source);


public slots:
    void evaluate(Case *c);
    
};

} // namespace ResOpt

#endif // LAUNCHER_H
