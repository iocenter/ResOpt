#include "launcher.h"

#include "model.h"
#include "reservoirsimulator.h"

namespace ResOpt
{

Launcher::Launcher(QObject *parent) :
    QObject(parent)
{
}


//-----------------------------------------------------------------------------------------------
// Running the model, calculating results
//-----------------------------------------------------------------------------------------------
void Launcher::evaluate()
{


    // running the reservoir simulator
    p_simulator->generateInputFiles(p_model);   // generating input based on the current Model
    p_simulator->launchSimulator();             // running the simulator
    p_simulator->readOutput(p_model);           // reading output from the simulator run, and setting to Model


    // calculating pressures in the Pipe network
    p_model->calculatePipePressures();

    // updating the constraints (this must be done after the pressure calc)
    p_model->updateConstraints();

    // updating the objective
    p_model->updateObjectiveValue();

    // changing the status to up to date
    //m_up_to_date = true;

    // letting the runner know the evaluation has finished
    emit finished();


}



} // namespace ResOpt
