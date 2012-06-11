#ifndef RUNNER_H
#define RUNNER_H


/**
 * @brief Class that controls the main execution of ResOpt
 *
 */

#include <tr1/memory>

#include <QString>
#include <QFile>

using std::tr1::dynamic_pointer_cast;
using std::tr1::shared_ptr;

namespace ResOpt
{

class Model;
class ReservoirSimulator;
class ModelReader;
class Optimizer;


/**
 * @brief Main execution class.
 *
 */
class Runner
{
private:
    ModelReader *p_reader;
    Model *p_model;
    ReservoirSimulator *p_simulator;
    Optimizer *p_optimizer;

    QFile *p_summary;
    int m_number_of_runs;
    bool m_up_to_date;



public:
    Runner(const QString &driver_file);
    ~Runner();


    /**
     * @brief Initializes the model and optimizer.
     * @details The Model is first read from the driver file. Then the ReservoirSimulator and Optimizer are initialized.
     *
     */
    void initialize();


    /**
     * @brief Starts the Optimizer
     * @details The function first checks if the Model has been initialized. If not initialize() is called. Then the Optimizer is started
     *          through Optimizer::start().
     *
     */
    void run();


    /**
     * @brief Runs the reservoir simulator on the current model.
     * @details This function is called by the optimizer at each iteration to evaluate objective and constraint values.
     *          First the simulator input files are generated. Then the simulator is launched, and output from the run is read. Then
     *          the pipe network pressures are calculated, and constraint values on separators and well BHPs are calculated. The Model
     *          is updated with the results from the run. These results can be read by the optimizer.
     *
     * @return bool
     */
    bool evaluate();

    void writeProblemDefToSummary();
    void writeIterationToSummary();

    // set functions

    void setSummaryFile(const QString &f);

    // get functions
    Model* model() {return p_model;}

    bool isUpToDate() {return m_up_to_date;}

};

} // namespace ResOpt

#endif // RUNNER_H

