#include "irlightconfigdialog.h"
#include "ui_irlightconfigdialog.h"
#include "mtcmanager.h"

IrLightConfigDialog::IrLightConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IrLightConfigDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("IR Light Config"));
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setModal(true);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &IrLightConfigDialog::acceptSlot);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &IrLightConfigDialog::rejectedSlot);
}

IrLightConfigDialog::~IrLightConfigDialog()
{
    delete ui;
}

void IrLightConfigDialog::setConfig(int light, int closeTime)
{
    ui->irLightSpinBox->setValue(light);
    ui->irCloseTimeSpinBox->setValue(closeTime);
}

void IrLightConfigDialog::acceptSlot()
{
    MtcManager::instance()->setIrLight(ui->irLightSpinBox->value(),ui->irCloseTimeSpinBox->value());
    this->close();
}

void IrLightConfigDialog::rejectedSlot()
{
    this->close();
}
