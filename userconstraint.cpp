#include "userconstraint.h"

#include <iostream>

#include "constraint.h"
#include "model.h"
#include "well.h"
#include "productionwell.h"
#include "pipeconnection.h"
#include "pipe.h"
#include "separator.h"
#include "stream.h"
#include "intvariable.h"
#include "realvariable.h"
#include "binaryvariable.h"

using std::cout;
using std::endl;

namespace ResOpt
{

UserConstraint::UserConstraint(Model *m)
    : p_model(m)
{

    p_constraint = shared_ptr<Constraint>(new Constraint());
}

UserConstraint::UserConstraint(const UserConstraint &uc, Model *new_model)
{
    // copying basic types
    m_expression = uc.m_expression;
    m_arguments = uc.m_arguments;
    m_operators = uc.m_operators;

    // setting the model
    p_model = new_model;

    // copying the constraint
    p_constraint = shared_ptr<Constraint>(new Constraint(*uc.p_constraint));

}


//-----------------------------------------------------------------------------------------------
// reads the bounds, sets up the constraint
//-----------------------------------------------------------------------------------------------
void UserConstraint::initialize()
{
    p_constraint->setMax(upperBound());
    p_constraint->setMin(lowerBound());

    p_constraint->setName("User defined constraint: " + m_expression);

    generateArguments();
}


//-----------------------------------------------------------------------------------------------
// returns the upper bound
//-----------------------------------------------------------------------------------------------
double UserConstraint::upperBound()
{
    QStringList list = m_expression.split("<");

    double max = list.at(list.size() -1).toDouble();

    return max;
}

//-----------------------------------------------------------------------------------------------
// returns the lower bound
//-----------------------------------------------------------------------------------------------
double UserConstraint::lowerBound()
{
    QStringList list = m_expression.split("<");

    double min = list.at(0).toDouble();

    return min;

}

//-----------------------------------------------------------------------------------------------
// extracts the arguments and operators from the expression
//-----------------------------------------------------------------------------------------------
void UserConstraint::generateArguments()
{
    // first getting a string containing all the arguments from the expression
    QStringList list = m_expression.split("<");


    // splitting arguments and operators
    QString args = list.at(1).trimmed();
    QStringList arg_list = args.split(QRegExp("\\s+"));


    // parsing
    OPER current_operator = PLUSS;


    for(int i = 0; i < arg_list.size(); i += 2)
    {
        m_arguments.push_back(arg_list.at(i));
        m_operators.push_back(current_operator);

        // finding the next operator
        if(i < (arg_list.size() - 1))
        {
            if(arg_list.at(i+1).startsWith("+")) current_operator = PLUSS;
            else if(arg_list.at(i+1).startsWith("-")) current_operator = MINUS;
        }


    }


}

//-----------------------------------------------------------------------------------------------
// updates the value of the constraint
//-----------------------------------------------------------------------------------------------
bool UserConstraint::update()
{
    bool ok = true;

    double value = 0;

    // looping through the list of arguments, adding them together
    for(int i = 0; i < m_arguments.size(); ++i)
    {
        if(m_operators.at(i) == PLUSS) value += resolveArgumentValue(m_arguments.at(i), &ok);
        else if(m_operators.at(i) == MINUS) value -= resolveArgumentValue(m_arguments.at(i), &ok);
    }

    // setting the value to the constraint
    p_constraint->setValue(value);

    return ok;

}

//-----------------------------------------------------------------------------------------------
// tries to find out what the argument represents in the model, and returns the value
//-----------------------------------------------------------------------------------------------
double UserConstraint::resolveArgumentValue(QString arg, bool *ok)
{
    // syntax:
    // type_id_comp_ts
    // WELL_prod1_OIL_1
    // PIPE_1_GAS_2
    // SEP_3_REMOVED_3

    double value = 0;

    bool ok_l = true;

    QStringList list = arg.split("_");

    QString type = list.at(0);          // well, pipe, separator
    QString id = list.at(1);            // well name or pipe number
    QString component = list.at(2);     // W, G, O, P, or REMOVED (separator only)
    int time_step = list.at(3).toInt(&ok_l);     // time step number according to master schedule
    if(!ok_l) error("The time step could not be converted to an integer: " + list.at(3));
    if(time_step >= p_model->numberOfMasterScheduleTimes() || time_step < 0) error("The entered time step is not valid: " + list.at(3));

    if(type.startsWith("WELL"))     // the argument is for a well
    {
        Well *w = 0;

        // looping through the wells, finding the correct one
        for(int i = 0; i < p_model->numberOfWells(); ++i)
        {
            if(p_model->well(i)->name().compare(id) == 0)   // this is the correct well
            {
                w = p_model->well(i);
                break;
            }
        }

        if(w == 0) error("Could not find a well named " + id);

        // now checking what type of rate to extract
        if(component.startsWith("G")) value = w->stream(time_step)->gasRate();
        else if(component.startsWith("O")) value = w->stream(time_step)->oilRate();
        else if(component.startsWith("W")) value = w->stream(time_step)->waterRate();
        else if(component.startsWith("P")) value = w->stream(time_step)->pressure();
        else if(component.startsWith("L")) // gas lift
        {
            // checking if this is a production well
            ProductionWell *prod_well = dynamic_cast<ProductionWell*>(w);

            if(prod_well != 0)
            {
                // checking if the well has gas lift
                if(prod_well->hasGasLift())
                {
                    // summing together the routing variables
                    double routing = 0;
                    for(int k = 0; k < prod_well->numberOfPipeConnections(); ++k)
                    {
                        routing += prod_well->pipeConnection(k)->variable()->value();
                    }

                    value = prod_well->gasLiftControl(time_step)->controlVar()->value() * routing;
                }
                else error("The well did not have gas lift when L was specified");
            }
            else error("L can only be specified for production wells");

        }
        else error("Type of component not recognized: " + component);

    } // well
    else if(type.startsWith("PIPE"))
    {
        Pipe *p = 0;

        int pipe_id = id.toInt(&ok_l);
        if(!ok_l) error("The pipe id could not be converted to an integer: " + id);

        // looping through the pipes, finding the correct one
        for(int i = 0; i < p_model->numberOfPipes(); ++i)
        {
            if(p_model->pipe(i)->number() == pipe_id)   // this is the correct well
            {
                p = p_model->pipe(i);
                break;
            }
        }

        if(p == 0) error("Could not find a pipe with id = " + id);

        // now checking what type of rate to extract
        if(component.startsWith("GAS")) value = p->stream(time_step)->gasRate();
        else if(component.startsWith("OIL")) value = p->stream(time_step)->oilRate();
        else if(component.startsWith("WAT")) value = p->stream(time_step)->waterRate();
        else if(component.startsWith("P")) value = p->stream(time_step)->pressure();
        else error("Type of component not recognized: " + component);


    } // pipe
    else if(type.startsWith("SEP"))
    {
        // first finding the pipe with the correct id, will later try to cast it to separator

        Pipe *p = 0;

        int pipe_id = id.toInt(&ok_l);
        if(!ok_l) error("The separator id could not be converted to an integer: " + id);

        // looping through the pipes, finding the correct one
        for(int i = 0; i < p_model->numberOfPipes(); ++i)
        {
            if(p_model->pipe(i)->number() == pipe_id)   // this is the correct well
            {
                p = p_model->pipe(i);
                break;
            }
        }

        if(p == 0) error("Could not find a separator with id = " + id);

        // now checking if it actually is a separator
        Separator *s = dynamic_cast<Separator*>(p);

        if(s == 0) error("Component #" + id + " is not a separator");

        // now checking what type of rate to extract
        if(component.startsWith("GAS")) value = s->stream(time_step)->gasRate();
        else if(component.startsWith("OIL")) value = s->stream(time_step)->oilRate();
        else if(component.startsWith("WAT")) value = s->stream(time_step)->waterRate();
        else if(component.startsWith("P")) value = s->stream(time_step)->pressure();
        else if(component.startsWith("REM")) // the amount of water removed from the separator
        {
            double qw_remove = 0;
            if(time_step >= s->installTime()->value())
            {
                // how much water should be removed
                double qw_remove = s->stream(time_step)->waterRate() * s->removeFraction()->value();
                if(qw_remove > s->removeCapacity()->value()) qw_remove = s->removeCapacity()->value();


            }

            value = qw_remove;

        }
        else error("Type of component not recognized: " + component);


    } // separator

    else error("The type of model component was not recognized: " + type);



    cout << "Value for argument " << arg.toAscii().data() << " = " << value << endl;

    return value;


}

//-----------------------------------------------------------------------------------------------
// terminates the program, prints an error message
//-----------------------------------------------------------------------------------------------
void UserConstraint::error(QString msg)
{
    cout << endl << "### Runtime Error ###" << endl
         << "Could not parse user defined constraint..." << endl
         << "Message: " << msg.toAscii().data() << endl << endl;

    exit(1);

}

} // namespace ResOpt
