#include "launcher.h"

#include "model.h"
#include "reservoirsimulator.h"
#include "case.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "objective.h"

namespace ResOpt
{

Launcher::Launcher(QObject *parent)
    : QObject(parent),
      p_model(0),
      p_simulator(0)
{
}

//-----------------------------------------------------------------------------------------------
// initializing
//-----------------------------------------------------------------------------------------------
bool Launcher::initialize()
{

    if(p_model != 0 && p_simulator != 0)
    {
        // resolving separator connections
        p_model->resolveCapacityConnections();

        // resolving the pipe routing (this must be done before each launch of the model)
        p_model->resolvePipeRouting();

        return true;

    }
    else return false;


}

//-----------------------------------------------------------------------------------------------
// Running the model, calculating results
//-----------------------------------------------------------------------------------------------
void Launcher::evaluate(Case *c)
{
    // setting the variable values according to the case

    for(int i = 0; i < p_model->realVariables().size(); ++i)
    {
        // need to add a check to see if anything requiering the reservoir simulator to launch has changed
        p_model->realVariables().at(i)->setValue(c->realVariableValue(i));
    }
    for(int i = 0; i < p_model->binaryVariables().size(); ++i)
    {
        p_model->binaryVariables().at(i)->setValue(c->binaryVariableValue(i));
    }


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

    // copying back the results to the case
    for(int i = 0; i < p_model->constraints().size(); ++i)
    {
        c->addConstraintValue(p_model->constraints().at(i)->value());
    }

    c->setObjectiveValue(p_model->objective()->value());

    // letting the runner know the evaluation has finished
    emit finished(this);


}



} // namespace ResOpt
