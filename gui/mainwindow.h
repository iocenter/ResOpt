#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QWidget>

#include "runner.h"

class QGraphicsView;
class QTabWidget;

using ResOpt::Runner;

namespace ResOptGui
{

class ModelScene;
class Console;
class Plot;
class InspectorOptimizer;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    ModelScene *p_model_scene;
    QGraphicsView *p_model_view;

    QTabWidget *p_tab_widget;

    Console *p_console;
    Plot *p_plot;
    InspectorOptimizer *p_obj_inpector;


    Runner *p_runner;


    void createMenus();

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();
    
signals:
    void sendMsg(QString);
    
public slots:
    void loadModel();
    void runModel();
    void onOptimizationFinished();
    void openOptimizerInspector();
    
};

} // namespace ResOptGui

#endif // MAINWINDOW_H
