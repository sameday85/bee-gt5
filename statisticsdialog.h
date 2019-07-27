#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <qvector.h>
#include "statistics.h"

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

signals:
    void resetStats();

private:
    QDialogButtonBox *buttonGrp1, *buttonGrp2;

    Statistics *stats;
    Statistics *perDictionaryStats;
    Statistics *lifetimeStats;
    QVector<Statistics*> *breakdown;
};

#endif // STATISTICSDIALOG_H
