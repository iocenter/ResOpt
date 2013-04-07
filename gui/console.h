#ifndef CONSOLE_H
#define CONSOLE_H

#include <QtGui/QDockWidget>
#include <QtGui/QTextEdit>



namespace ResOptGui
{

class Console : public QDockWidget
{
    Q_OBJECT

private:
    QTextEdit m_edit;


public:
    explicit Console(QWidget *parent = 0);
    
signals:
    
public slots:
    void printMsg(QString);
    
};

} // namespace

#endif // CONSOLE_H
