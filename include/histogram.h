#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>
#include <QtCharts>

class Histogram: public QMainWindow
{
public:
    Histogram(QWidget* parent, std::vector<float>);
    ~Histogram() override;
    void keyPressEvent(QKeyEvent* event) override;
};

#endif // HISTOGRAM_H
