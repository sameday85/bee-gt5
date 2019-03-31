#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include "statistics.h"

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(QWidget* parent, Statistics *stats_, Statistics *lifetimeStats_);

private:
    void setUpGUI();

public slots:
    void slotOnOk();

private:
    QDialogButtonBox *buttons;

    Statistics *stats;
    Statistics *lifetimeStats;
};

#endif // STATISTICSDIALOG_H