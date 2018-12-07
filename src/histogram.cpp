#include "../include/histogram.h"

Histogram::Histogram(QWidget* parent, std::vector<float> values)
    :QMainWindow(parent)
{
    QBarSet* set0 = new QBarSet("Values");
    for(const auto& v: values)
        *set0 << double(v);

    set0->setPen(QPen(QColor(Qt::black)));

    QBarSeries* serie = new QBarSeries();
    serie->append(set0);

    QChart* chart = new QChart();
    chart->addSeries(serie);
    chart->setTitle("Dihedrals angles");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis* axis = new QBarCategoryAxis();
    chart->createDefaultAxes();
    chart->setAxisX(axis, serie);

    chart->legend()->setVisible(false);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView* view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);

    setWindowTitle("Histogram");
    setCentralWidget(view);
    resize(700, 300);
}

Histogram::~Histogram()
{

}

void
Histogram::keyPressEvent(QKeyEvent* event)
{
    if( event->key() == Qt::Key_Escape )
        close();
}

