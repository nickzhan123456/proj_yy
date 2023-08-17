#include "qrcodeswitchdialog.h"
#include "ui_qrcodeswitchdialog.h"
#include "mtcmanager.h"

QRCodeSwitchDialog::QRCodeSwitchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QRCodeSwitchDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("QRCode Switch"));
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setModal(true);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QRCodeSwitchDialog::acceptSlot);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QRCodeSwitchDialog::rejectedSlot);
}

QRCodeSwitchDialog::~QRCodeSwitchDialog()
{
    delete ui;
}

void QRCodeSwitchDialog::setQRCodeStatus(int mode, int interval)
{
    bool enable = mode == 1 ? true : false;
    ui->checkBox->setChecked(enable);
    ui->spinBox->setValue(interval);
}

void QRCodeSwitchDialog::acceptSlot()
{
    
    int mode = ui->checkBox->isChecked() ? 1 : 0;
    MtcManager::instance()->setQRCodeSwitch(mode,ui->spinBox->value());
    this->close();
}

void QRCodeSwitchDialog::rejectedSlot()
{
    this->close();
}
