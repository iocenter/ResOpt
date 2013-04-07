#include "inspectoroptimizer.h"

#include "runner.h"
#include "optimizer.h"
#include "nomadoptimizer.h"
#include "bonminoptimizer.h"
#include "runonceoptimizer.h"

using ResOpt::Optimizer;
using ResOpt::BonminOptimizer;
using ResOpt::NomadOptimizer;
using ResOpt::RunonceOptimizer;

#include "QtGui/QGridLayout"
#include "QtGui/QLabel"

namespace ResOptGui
{

InspectorOptimizer::InspectorOptimizer(Runner *r, QWidget *parent) :
    QWidget(0),
    p_runner(r),
    m_alg_in_runner(0)
{

    construct();
    show();
}


//-----------------------------------------------------------------------------------------------
// constructs the view
//-----------------------------------------------------------------------------------------------
void InspectorOptimizer::construct()
{

    QGridLayout *layout = new QGridLayout();
    setLayout(layout);


    // ---- algorithm ------
    layout->addWidget(new QLabel("Optimization algorithm: ", this), 0, 0, 1, 2);

    p_algorithm = new QComboBox(this);

    p_algorithm->addItem("Bonmin");
    p_algorithm->addItem("NOMAD");
    p_algorithm->addItem("Run Once");

    // finding out what type of optimizer is currently used

    BonminOptimizer *l_bonmin = dynamic_cast<BonminOptimizer*>(p_runner->optimizer());
    NomadOptimizer *l_nomad = dynamic_cast<NomadOptimizer*>(p_runner->optimizer());
    RunonceOptimizer *l_runonce = dynamic_cast<RunonceOptimizer*>(p_runner->optimizer());

    if(l_bonmin != 0)
    {
        p_algorithm->setCurrentIndex(0);
        m_alg_in_runner = 0;
    }
    else if(l_nomad != 0)
    {
        p_algorithm->setCurrentIndex(1);
        m_alg_in_runner = 1;
    }
    else if(l_runonce != 0)
    {
        p_algorithm->setCurrentIndex(2);
        m_alg_in_runner = 2;
    }


    layout->addWidget(p_algorithm, 0, 2);

    // --- max iterations ----
    layout->addWidget(new QLabel("Maximum number of iterations: ", this), 1, 0, 1, 2);

    p_max_iter = new QLineEdit(this);
    p_max_iter->setText(QString::number(p_runner->optimizer()->maxIterations()));
    layout->addWidget(p_max_iter, 1, 2);


    // ---- buttons ----
    p_btn_ok = new QPushButton("Ok", this);
    connect(p_btn_ok, SIGNAL(clicked()), this, SLOT(saveAndClose()));
    layout->addWidget(p_btn_ok, 2, 0);

    p_btn_close = new QPushButton("Close", this);
    connect(p_btn_close, SIGNAL(clicked()), this, SLOT(close()));
    layout->addWidget(p_btn_close, 2, 1);


}

//-----------------------------------------------------------------------------------------------
// saves changes to the optimizer, and closes
//-----------------------------------------------------------------------------------------------
void InspectorOptimizer::saveAndClose()
{
    // setting the max iterations
    p_runner->optimizer()->setMaxIterations(p_max_iter->text().toInt());

    // seeing if the optimizer type has changed
    if(m_alg_in_runner != p_algorithm->currentIndex())
    {
        Optimizer *o = 0;

        if(p_algorithm->currentIndex() == 0) o = new BonminOptimizer(p_runner);
        else if(p_algorithm->currentIndex() == 1) o = new NomadOptimizer(p_runner);
        else if(p_algorithm->currentIndex() == 2) o = new RunonceOptimizer(p_runner);

        // setting values to the new optimizer
        o->setMaxIterations(p_runner->optimizer()->maxIterations());
        o->setParallelRuns(p_runner->optimizer()->parallelRuns());
        o->setPerturbationSize(p_runner->optimizer()->pertrurbationSize());

        // deleting the old optimizer
        p_runner->optimizer()->setParent(0);
        p_runner->optimizer()->deleteLater();

        // setting the new optimizer
        p_runner->setOptimizer(o);

        // initializing the new optimizer
        p_runner->optimizer()->initialize();


    }

    // closing
    close();
}

} // namespace
