#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QWidget>

#include "runner.h"

class QGraphicsView;

using ResOpt::Runner;

namespace ResOptGui
{

class ModelScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    ModelScene *p_model_scene;
    QGraphicsView *p_model_view;
    Runner *p_runner;


    void createMenus();

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();
    
signals:
    
public slots:
    void loadModel();
    
};

} // namespace ResOptGui

#endif // MAINWINDOW_H
