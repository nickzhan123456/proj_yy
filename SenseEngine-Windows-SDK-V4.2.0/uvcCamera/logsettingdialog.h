#ifndef LOGSETTINGDIALOG_H
#define LOGSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class LogSettingDialog;
}

class LogSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogSettingDialog(QWidget *parent = nullptr);
    ~LogSettingDialog();
    void setLogConfig(int logLever ,int logTarget);

public slots:
    void acceptSlot();
    void rejectedSlot();

private:
    Ui::LogSettingDialog *ui;
};

#endif // LOGSETTINGDIALOG_H
