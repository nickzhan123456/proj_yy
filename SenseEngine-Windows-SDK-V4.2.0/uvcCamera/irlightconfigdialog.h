#ifndef IRLIGHTCONFIGDIALOG_H
#define IRLIGHTCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class IrLightConfigDialog;
}

class IrLightConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IrLightConfigDialog(QWidget *parent = nullptr);
    ~IrLightConfigDialog();
    void setConfig(int light, int closeTime);

public slots:
    void acceptSlot();
    void rejectedSlot();

private:
    Ui::IrLightConfigDialog *ui;
};

#endif // IRLIGHTCONFIGDIALOG_H
