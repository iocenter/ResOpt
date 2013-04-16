#include "bonminoptimizer.h"

#include "bonmininterface.h"
#include "bonmininterfacegradients.h"
#include "adjointscoupledmodel.h"
#include "runner.h"

#include <iostream>

#include <QString>


using std::cout;
using std::endl;

namespace ResOpt
{


BonminOptimizer::BonminOptimizer(Runner *r)
    : Optimizer(r)
{}

BonminOptimizer::~BonminOptimizer()
{
    // nothing to do, the SmartPtr will delete itself
}

//-----------------------------------------------------------------------------------------------
// Initializes the optimizer
//-----------------------------------------------------------------------------------------------
void BonminOptimizer::initialize()
{
    cout << "Initializing bonmin..." << endl;


    // setting up the TMINLP
    SmartPtr<TMINLP> tminlp;

    // if the model supports adjoints, BonminInterfaceGradients is used
    AdjointsCoupledModel *am = dynamic_cast<AdjointsCoupledModel*>(runner()->model());
    if(am != 0) tminlp = new BonminInterfaceGradients(this);
    else tminlp = new BonminInterface(this);



    // initializing bonmin
    m_bonmin.initializeOptionsAndJournalist();

    // Here we can change the default value of some Bonmin or Ipopt option
    //  bonmin.options()->SetNumericValue("bonmin.time_limit", 5); //changes bonmin's time limit
    m_bonmin.options()->SetStringValue("mu_oracle","loqo");

    // Options can also be set by using a string with a format similar to the bonmin.opt file
    m_bonmin.readOptionsString("bonmin.algorithm B-BB\n");

    m_bonmin.readOptionsString("tol 6.0\n");
    m_bonmin.readOptionsString("compl_inf_tol 0.1\n");
    m_bonmin.readOptionsString("dual_inf_tol 300000.0\n");
    m_bonmin.readOptionsString("constr_viol_tol 0.1\n");
    QString s_max_iter = "max_iter " + QString::number(maxIterations()) + "\n";
    m_bonmin.readOptionsString(s_max_iter.toStdString());
    m_bonmin.readOptionsString("output_file output/ipopt.out\n");
    m_bonmin.readOptionsString("bonmin.file_solution yes\n");

    //Here we read several option files
    //bonmin.readOptionsFile("Mybonmin.opt");
    //m_bonmin.readOptionsFile("bonmin.opt");
    m_bonmin.readOptionsFile();// This reads the default file "bonmin.opt"

    m_bonmin.readOptionsString("hessian_approximation limited-memory\n");

    // initializing the TMINLP
    m_bonmin.initialize(GetRawPtr(tminlp));

    // changing the status to initialized
    setInitialized(true);



}

//-----------------------------------------------------------------------------------------------
// Starts the optimization
//-----------------------------------------------------------------------------------------------
void BonminOptimizer::start()
{
    // checking if everything is initialized
    if(!isInitialized()) initialize();

    //Set up done, now let's branch and bound
      double time1 = CoinCpuTime();
      try
      {


          cout << "Starting bonmin..." << endl;

          Bab bb;
          bb(m_bonmin);//process parameter file using Ipopt and do branch and bound using Cbc


      }
      catch(TNLPSolver::UnsolvedError *E) {
        //There has been a failure to solve a problem with Ipopt.
        std::cerr<<"Ipopt has failed to solve a problem"<<std::endl;
      }
      catch(OsiTMINLPInterface::SimpleError &E) {
        std::cerr<<E.className()<<"::"<<E.methodName()
             <<std::endl
             <<E.message()<<std::endl;
      }
      catch(CoinError &E) {
        std::cerr<<E.className()<<"::"<<E.methodName()
             <<std::endl
             <<E.message()<<std::endl;
      }


      // letting the runner know the optimization has finished
      emit finished();
}

} // namespace ResOpt
