#ifndef INSPECTOROPTIMIZER_H
#define INSPECTOROPTIMIZER_H

#include <QtGui/QWidget>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

namespace ResOpt
{
class Runner;
}

using ResOpt::Runner;

namespace ResOptGui
{

class InspectorOptimizer : public QWidget
{
    Q_OBJECT
private:
    Runner *p_runner;

    QComboBox *p_algorithm;
    QLineEdit *p_max_iter;

    QPushButton *p_btn_close;
    QPushButton *p_btn_ok;

    int m_alg_in_runner;


    void construct();

public:
    explicit InspectorOptimizer(Runner *r, QWidget *parent = 0);
    
signals:
    
public slots:
    void saveAndClose();
    
};

} // namespace

#endif // INSPECTOROPTIMIZER_H
