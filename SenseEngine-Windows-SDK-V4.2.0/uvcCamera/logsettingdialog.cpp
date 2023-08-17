#include "logsettingdialog.h"
#include "ui_logsettingdialog.h"
#include "mtcmanager.h"

LogSettingDialog::LogSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogSettingDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Log Setting"));
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setModal(true);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &LogSettingDialog::acceptSlot);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &LogSettingDialog::rejectedSlot);
}

LogSettingDialog::~LogSettingDialog()
{
    delete ui;
}

void LogSettingDialog::setLogConfig(int logLever ,int logTarget)
{
    ui->comboBoxLogLever->setCurrentIndex(logLever);
    ui->comboBoxLogTraget->setCurrentIndex(logTarget);
}

void LogSettingDialog::acceptSlot()
{
    MtcManager::instance()->setLogConfig(ui->comboBoxLogLever->currentIndex(),ui->comboBoxLogTraget->currentIndex());
    this->close();
}

void LogSettingDialog::rejectedSlot()
{
    this->close();
}
