#include "mainwindow.h"
#include "modelscene.h"

#include <QtGui/QAction>
#include <QtGui/QKeySequence>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGraphicsView>
#include <QtGui/QFileDialog>

#include <QDir>




namespace ResOptGui
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    p_runner(0)
{

    // setting up the model scene
    p_model_scene = new ModelScene(this);

    // setting up the central widget
   // QWidget *widget = new QWidget;

    //setCentralWidget(p_model_view);


    QHBoxLayout *layout = new QHBoxLayout;

    p_model_view = new QGraphicsView(p_model_scene);
    p_model_view->setFixedSize(800, 600);
    layout->addWidget(p_model_view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    setWindowTitle("ResOpt GUI");

    // creating the menus
    createMenus();

}

MainWindow::~MainWindow()
{
    if(p_runner != 0) delete p_runner;

    p_runner = 0;
}


//-----------------------------------------------------------------------------------------------
// creates the menu items
//-----------------------------------------------------------------------------------------------
void MainWindow::createMenus()
{
    // creating file menu
    QMenu *p_file_menu = menuBar()->addMenu("File");

    QAction *p_act_load =  p_file_menu->addAction("Load Project");


    connect(p_act_load, SIGNAL(triggered()), this, SLOT(loadModel()));

}


//-----------------------------------------------------------------------------------------------
// Select model file, and set up runner
//-----------------------------------------------------------------------------------------------
void MainWindow::loadModel()
{
    // letting the user specify the driver file
    QString fileName = QFileDialog::getOpenFileName(this, "Load Project", QDir::currentPath(), "ResOpt Driver File (*.dat)");

    // setting up the runner with the specified driver file
    if(p_runner == 0)
    {
        p_runner = new Runner(fileName);

        p_runner->initialize();

        p_model_scene->buildSceneFromModel(p_runner->model());
    }

}

} // namespace
