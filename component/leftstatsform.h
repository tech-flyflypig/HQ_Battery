#ifndef LEFTSTATSFORM_H
#define LEFTSTATSFORM_H

#include <QWidget>

namespace Ui
{
    class LeftStatsForm;
}

class LeftStatsForm : public QWidget
{
    Q_OBJECT

public:
    explicit LeftStatsForm(QWidget *parent = nullptr);
    ~LeftStatsForm();

private slots:
    void updateStats();

private:
    Ui::LeftStatsForm *ui;
};

#endif // LEFTSTATSFORM_H
