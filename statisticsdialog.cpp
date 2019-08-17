#include "statisticsdialog.h"
#include <QLabel>
#include <QDateTime>
#include <QGridLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QDebug>

QT_CHARTS_USE_NAMESPACE

StatisticsDialog::StatisticsDialog(QWidget * parent, Statistics *stats_, Statistics *perDictionary_, Statistics *lifetimeStats_, QVector<Statistics*> *breakdown_) :
    QDialog(parent)
{
    stats = stats_;
    perDictionaryStats = perDictionary_;
    lifetimeStats = lifetimeStats_;
    breakdown = breakdown_;

    setUpGUI();
    setWindowTitle( tr("Statistics") );
    setModal( true );
    //minimum 634x482
    int nWidth = 800;
    int nHeight= 600;
    if (parent != NULL)
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
                    parent->y() + parent->height()/2 - nHeight/2,
                    nWidth, nHeight);
    else
        resize(nWidth, nHeight);
}

StatisticsDialog::~StatisticsDialog() {
    for (int i = 0; i < breakdown->size(); ++i)
        delete breakdown->at(i);
    delete breakdown;

    delete perDictionaryStats;
    delete lifetimeStats;
}

void StatisticsDialog::setUpGUI() {
    // set up the layout
    QGridLayout *formGridLayout = new QGridLayout( this );
    //breakdown chart
    {
        QBarSet *set0 = new QBarSet("Asked");
        QBarSet *set1 = new QBarSet("Correct");
        QStringList categories;
        int maxY = 0;
        for (int i = 0; i < breakdown->size(); ++i) {
            Statistics *statis = breakdown->at(i);
            *set0 << statis->getAsked();
            *set1 << statis->getAnswered();

            QDateTime dt;
            dt.setMSecsSinceEpoch(statis->getTime());
            categories << dt.toString("MM/dd");

            if (statis->getAsked() > maxY)
                maxY = statis->getAsked();
        }
        QBarSeries *series = new QBarSeries(this);
        series->append(set0);
        series->append(set1);

        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Daily Activities");
        chart->setAnimationOptions(QChart::SeriesAnimations);

        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0, maxY);
        axisY->setLabelFormat("%i");
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        QHBoxLayout *hBoxLayout = new QHBoxLayout();
        hBoxLayout->addWidget(chartView);
        formGridLayout->addLayout(hBoxLayout, 0, 0, 1, 3, Qt::AlignCenter);
    }
    //today's statistics
    {
        QPieSeries *series = new QPieSeries();
        series->append("Correct", stats ? stats->getAnswered() : 0);
        series->append("Failed", stats ? stats->getAsked() - stats->getAnswered() : 0);
        {
            QPieSlice *slice = series->slices().at(0);
            slice->setBrush(Qt::green);
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

        formGridLayout->addWidget(chartView, 1, 0);
        QLabel *label = new QLabel (this);
        if (stats)
            label->setText(QString::asprintf("%d of %d: %.2f%%", stats->getAnswered(), stats->getAsked(), stats->getCorrectPercentage()));
        else
            label->setText("N/A");
        formGridLayout->addWidget(label, 2, 0, Qt::AlignCenter);
    }
    //per dictionary statistics
    {
        QPieSeries *series = new QPieSeries();
        series->append("Correct", perDictionaryStats->getAnswered());
        series->append("Failed", perDictionaryStats->getAsked() - perDictionaryStats->getAnswered());
        {
            QPieSlice *slice = series->slices().at(0);
            slice->setBrush(Qt::green);
        }
        {
            QPieSlice *slice = series->slices().at(1);
            slice->setBrush(Qt::red);
        }
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Current List");
        chart->legend()->hide();

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        formGridLayout->addWidget(chartView, 1, 1);
        QLabel *label = new QLabel (this);
        label->setText(QString::asprintf("%d of %d: %.2f%%", perDictionaryStats->getAnswered(), perDictionaryStats->getAsked(), perDictionaryStats->getCorrectPercentage()));
        formGridLayout->addWidget(label, 2, 1, Qt::AlignCenter);
    }
    //lifetime statistics
    {
        QPieSeries *series = new QPieSeries();
        series->append("Correct", lifetimeStats->getAnswered());
        series->append("Failed", lifetimeStats->getAsked() - lifetimeStats->getAnswered());
        {
            QPieSlice *slice = series->slices().at(0);
            slice->setBrush(Qt::green);
        }
        {
            QPieSlice *slice = series->slices().at(1);
            slice->setBrush(Qt::red);
        }
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("History");
        chart->legend()->hide();

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        formGridLayout->addWidget(chartView, 1, 2);
        QLabel *label = new QLabel (this);
        label->setText(QString::asprintf("%d of %d: %.2f%%", lifetimeStats->getAnswered(), lifetimeStats->getAsked(), lifetimeStats->getCorrectPercentage()));
        formGridLayout->addWidget(label, 2, 2, Qt::AlignCenter);
    }

    buttonGrp1 = new QDialogButtonBox( this );
    buttonGrp1->addButton( QDialogButtonBox::Reset);
    connect( buttonGrp1->button( QDialogButtonBox::Reset ),
             SIGNAL (clicked()),
             this,
             SLOT (slotOnReset()) );
    //addWidget(QWidget * widget, int fromRow, int fromColumn, int rowSpan, int columnSpan, Qt::Alignment alignment = 0)
    formGridLayout->addWidget(buttonGrp1, 3, 1, 1, 1, Qt::AlignCenter);

    buttonGrp2 = new QDialogButtonBox( this );
    buttonGrp2->addButton( QDialogButtonBox::Ok);
    connect( buttonGrp2->button( QDialogButtonBox::Ok ),
             SIGNAL (clicked()),
             this,
             SLOT (slotOnOk()) );
    //addWidget(QWidget * widget, int fromRow, int fromColumn, int rowSpan, int columnSpan, Qt::Alignment alignment = 0)
    formGridLayout->addWidget(buttonGrp2, 3, 2, 1, 1);

    formGridLayout->setRowStretch(0, 4);
    formGridLayout->setRowStretch(1, 2);
    formGridLayout->setRowStretch(2, 1);
    formGridLayout->setRowStretch(3, 1);
    setLayout( formGridLayout );
}

void StatisticsDialog::slotOnOk() {
    //qDebug() << size().width() << size().height();
    close();
}

void StatisticsDialog::slotOnReset() {
    emit resetStats();
    close();
}
