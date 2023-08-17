#ifndef QRCODESWITCHDIALOG_H
#define QRCODESWITCHDIALOG_H

#include <QDialog>

namespace Ui {
class QRCodeSwitchDialog;
}

class QRCodeSwitchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QRCodeSwitchDialog(QWidget *parent = nullptr);
    ~QRCodeSwitchDialog();
    void setQRCodeStatus(int mode, int interval);

public slots:
    void acceptSlot();
    void rejectedSlot();

private:
    Ui::QRCodeSwitchDialog *ui;
};

#endif // QRCODESWITCHDIALOG_H
