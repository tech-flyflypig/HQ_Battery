#ifndef REVISEPOWERFORM_H
#define REVISEPOWERFORM_H

#include <QWidget>
#include "Struct.h"
#include <QMouseEvent>
namespace Ui
{
    class RevisePowerForm;
}

class RevisePowerForm : public QWidget
{
    Q_OBJECT

public:
    explicit RevisePowerForm(const battery_info&, QWidget* parent = nullptr);
    ~RevisePowerForm();
private slots:
    void on_btn_close_clicked();

    void on_cbt_com_currentIndexChanged(const QString& arg1);

    void on_btn_sure_clicked();

private:
    void initForm();

private:
    Ui::RevisePowerForm* ui;
    battery_info m_battery;
    QPoint clickPos;

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

signals:
    void sig_init_sql();
};

#endif // REVISEPOWERFORM_H
