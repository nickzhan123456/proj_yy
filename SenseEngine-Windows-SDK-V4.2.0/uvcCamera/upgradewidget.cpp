#include "upgradewidget.h"
#include "ui_upgradewidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QCryptographicHash>
#include "XFFmpeg.h"
#include "XVideoThread.h"
#include "qmtc/mtcmanager.h"

UpgradeWidget::UpgradeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpgradeWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowModal, true);
    this->setWindowTitle("Upgrade");
    ui->okBtn->setEnabled(false);
    connect(ui->selectBtn,&QPushButton::clicked,this,&UpgradeWidget::selectFileSlot);
    connect(ui->upgradeBtn,&QPushButton::clicked,this,&UpgradeWidget::upgradeSlot);
    connect(ui->cancelBtn,&QPushButton::clicked,this,&UpgradeWidget::cancelSlot);
    connect(ui->okBtn,&QPushButton::clicked,this,&UpgradeWidget::cancelSlot);
    connect(MtcManager::instance(),&MtcManager::uploadPackageProgress,this,&UpgradeWidget::updateUploadProgress);
    connect(MtcManager::instance(),&MtcManager::setUploadRange,this,&UpgradeWidget::setUploadRange);
    connect(MtcManager::instance(),&MtcManager::uploadStatus,this,&UpgradeWidget::uploadStatusSlots);
}

UpgradeWidget::~UpgradeWidget()
{
    delete ui;
}

void UpgradeWidget::selectFileSlot()
{
    QString selectedFilter;
    m_selectFile.clear();
    m_selectFile = QFileDialog::getOpenFileName(
                this, tr("Add Batch"),
                "/home",
                tr("Bin Files (*.bin)"),
                &selectedFilter);
    if (!m_selectFile.isEmpty()) {
        //qDebug() << "upgrade file : " << m_selectFile << "file size : " << QFileInfo(m_selectFile).size();
        ui->progressBar->reset();

        ui->progressBar->setRange(0,QFileInfo(m_selectFile).size());

        ui->textEdit->clear();
        ui->textEdit->append(QString("Select %1 upgrade file,please clicked upgrade!").arg(m_selectFile));
    }
}

void UpgradeWidget::upgradeSlot()
{
    if(m_selectFile.isEmpty()){
        QMessageBox::warning(this,"SenseEnginie Camera","please select upgrade file!");
        return;
    }

    ui->okBtn->setEnabled(true);
    ui->upgradeBtn->setEnabled(false);
    MtcManager::instance()->upgrade(m_selectFile);
    m_selectFile.clear();
}

void UpgradeWidget::cancelSlot()
{
    MtcManager::instance()->cancelUpgradeBatch();
    this->close();
}

void UpgradeWidget::closeEvent(QCloseEvent *event)
{
    MtcManager::instance()->cancelUpgradeBatch();
    this->close();
}

void UpgradeWidget::updateUploadProgress(int value)
{
    ui->progressBar->setValue(value);
    //qDebug()<<"upload progress:"<<value;
}

void UpgradeWidget::setUploadRange(int value)
{

    ui->progressBar->setRange(0,value);
    //qDebug()<<"upload total:"<<value;
}

void UpgradeWidget::uploadStatusSlots(bool isComplete, QString msg, bool isSuccess)
{
    ui->upgradeBtn->setEnabled(true);
    ui->textEdit->append(msg);
    if(isComplete && isSuccess)
    {
        emit upgradeSuccess();
    }
}
