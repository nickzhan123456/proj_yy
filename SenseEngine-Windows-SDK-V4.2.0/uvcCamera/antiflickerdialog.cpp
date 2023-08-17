#include "antiflickerdialog.h"
#include "ui_antiflickerdialog.h"
#include "mtcmanager.h"

AntiFlickerDialog::AntiFlickerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AntiFlickerDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Anti Flicker Setting"));
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setModal(true);
    m_enableGroup = new QButtonGroup;
    m_frequencyGroup = new QButtonGroup;
    m_enableGroup->addButton(ui->radioButtonDisable);
    m_enableGroup->addButton(ui->radioButtonEnable);
    m_frequencyGroup->addButton(ui->radioButton50Hz);
    m_frequencyGroup->addButton(ui->radioButton60Hz);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AntiFlickerDialog::acceptSlot);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AntiFlickerDialog::rejectedSlot);
}

AntiFlickerDialog::~AntiFlickerDialog()
{
    if(m_enableGroup)
    {
        delete m_enableGroup;
        m_enableGroup = nullptr;
    }

    if(m_frequencyGroup)
    {
        delete m_frequencyGroup;
        m_frequencyGroup = nullptr;
    }

    delete ui;
}

void AntiFlickerDialog::setAntiFlicker(int cameraId, int enable, int frequency)
{
    m_cameraId = cameraId;
    ui->radioButtonDisable->setChecked(enable);
    ui->radioButtonEnable->setChecked(!enable);
    ui->radioButton50Hz->setChecked(!frequency);
    ui->radioButton60Hz->setChecked(frequency);
}

void AntiFlickerDialog::acceptSlot()
{
    MtcManager::instance()->antiFlicker((int)ui->radioButtonDisable->isChecked(),
                                        m_cameraId,
                                        (int)ui->radioButton60Hz->isChecked());
    this->close();
}

void AntiFlickerDialog::rejectedSlot()
{
    this->close();
}
