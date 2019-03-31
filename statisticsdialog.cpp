#include "statisticsdialog.h"
#include <QGridLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

QT_CHARTS_USE_NAMESPACE

StatisticsDialog::StatisticsDialog(QWidget * parent, Statistics *stats_, Statistics *lifetimeStats_) :
    QDialog(parent)
{
    stats = stats_;
    lifetimeStats = lifetimeStats_;

    setUpGUI();
    setWindowTitle( tr("Statistics") );
    setModal( true );

    int nWidth = 400;
    int nHeight= 200;
    if (parent != NULL)
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
                    parent->y() + parent->height()/2 - nHeight/2,
                    nWidth, nHeight);
    else
        resize(nWidth, nHeight);
}

void StatisticsDialog::setUpGUI() {
    // set up the layout
    QGridLayout *formGridLayout = new QGridLayout( this );
    //today's statistics
    {
        QPieSeries *series = new QPieSeries();
        series->append("Correct", stats->getCorrect());
        series->append("Failed", stats->getAsked() - stats->getCorrect());
        {
            QPieSlice *slice = series->slices().at(0);
            slice->setBrush(Qt::blue);
        }
        {
            QPieSlice *slice = series->slices().at(1);
            slice->setBrush(Qt::red);
        }
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Today");
        chart->legend()->hide();

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        formGridLayout->addWidget(chartView, 0, 0);
    }
    //lifetime statistics
    {
        QPieSeries *series = new QPieSeries();
        series->append("Correct", lifetimeStats->getCorrect());
        series->append("Failed", lifetimeStats->getAsked() - lifetimeStats->getCorrect());
        {
            QPieSlice *slice = series->slices().at(0);
            slice->setBrush(Qt::blue);
        }
        {
            QPieSlice *slice = series->slices().at(1);
            slice->setBrush(Qt::red);
        }
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Lifetime");
        chart->legend()->hide();

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        formGridLayout->addWidget(chartView, 0, 1);
    }

    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok ); //practice

    connect( buttons->button( QDialogButtonBox::Ok ),
             SIGNAL (clicked()),
             this,
             SLOT (slotOnOk()) );

    //addWidget(QWidget * widget, int fromRow, int fromColumn, int rowSpan, int columnSpan, Qt::Alignment alignment = 0)
    formGridLayout->addWidget(buttons, 1, 0, 1, 2 );

    setLayout( formGridLayout );
}

void StatisticsDialog::slotOnOk() {
    close();
}

