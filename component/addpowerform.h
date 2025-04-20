#ifndef ADDPOWERFORM_H
#define ADDPOWERFORM_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui
{
    class AddPowerForm;
}

class AddPowerForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddPowerForm(QWidget* parent = nullptr);
    ~AddPowerForm();

private slots:
    void on_btn_close_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void initForm();
    void on_btn_sure_clicked();

    void on_cbt_com_currentIndexChanged(const QString& arg1);

    void on_comboBox_currentIndexChanged(const QString& arg1);

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
signals:
    void sig_init_sql();

private:
    Ui::AddPowerForm* ui;
    QPoint clickPos;
};

#endif // ADDPOWERFORM_H
