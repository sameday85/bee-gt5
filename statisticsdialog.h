#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QtCharts/QBarSet>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <qvector.h>
#include "statistics.h"

QT_CHARTS_USE_NAMESPACE

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(QWidget* parent, Statistics *stats_, Statistics *perDictionary_, Statistics *lifetimeStats_, QVector<Statistics*> *breakdown);
    ~StatisticsDialog();

private:
    void setUpGUI();

public slots:
    void slotOnOk();
    void slotOnReset();
    void slotOnBarClicked(int, QBarSet*);

signals:
    void resetStats();

private:
    QDialogButtonBox *buttonGrp1, *buttonGrp2;
    QLabel *labelInfo;

    Statistics *stats;
    Statistics *perDictionaryStats;
    Statistics *lifetimeStats;
    QVector<Statistics*> *breakdown;
};

#endif // STATISTICSDIALOG_H
