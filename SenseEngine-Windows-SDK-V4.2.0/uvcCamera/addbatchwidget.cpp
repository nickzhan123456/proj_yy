#include "addbatchwidget.h"
#include "ui_addbatchwidget.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "qmtc/mtcmanager.h"

AddBatchWidget::AddBatchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddBatchWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowModal, true);
    this->setWindowTitle("Add Batch");
    ui->okBtn->setEnabled(false);
    connect(ui->selectBtn,&QPushButton::clicked,this,&AddBatchWidget::selectFilesSlot);
    connect(ui->uploadBtn,&QPushButton::clicked,this,&AddBatchWidget::uploadSlot);
    connect(ui->cancelBtn,&QPushButton::clicked,this,&AddBatchWidget::cancelSlot);
    connect(ui->okBtn,&QPushButton::clicked,this,&AddBatchWidget::cancelSlot);
    connect(MtcManager::instance(),&MtcManager::addImageBatchResultSignal,this,&AddBatchWidget::addBatchResultSlot);
}

AddBatchWidget::~AddBatchWidget()
{
    MtcManager::instance()->cancelAddImageBatch();
    //qDebug() << "AddBatchWidget::~AddBatchWidget";
    delete ui;
}

void AddBatchWidget::selectFilesSlot()
{
    selectFiles.clear();
    m_currentIndex = 0;
    m_successCount = 0;
    selectFiles = QFileDialog::getOpenFileNames(
                this, tr("Add Batch"),
                "/home",
                tr("JPEG Files (*.jpg *.jpeg)"));
    if (!selectFiles.isEmpty()) {
        //qDebug() << "Add Batch : " << selectFiles;
        ui->progressBar->reset();
        ui->progressBar->setRange(0,selectFiles.count());
        ui->textEdit->clear();
        ui->textEdit->append(QString("Select %1 picture,please clicked upload!").arg(selectFiles.count()));
        m_isCancel = true;
    }
}

void AddBatchWidget::uploadSlot()
{
    if(selectFiles.count() == 0){
        QMessageBox::warning(this,"SenseEnginie Camera","please select files!");
        return;
    }
    m_isCancel = false;
    ui->selectBtn->setEnabled(false);    
    ui->uploadBtn->setEnabled(false);
    MtcManager::instance()->closeCamera();
    MtcManager::instance()->addImageBatch(selectFiles.at(m_currentIndex));
}

void AddBatchWidget::cancelSlot()
{
    m_isCancel = true;
    MtcManager::instance()->openCamera();
    selectFiles.clear();
    this->close();
}

void AddBatchWidget::closeEvent(QCloseEvent *event)
{
    m_isCancel = true;
    MtcManager::instance()->openCamera();
    selectFiles.clear();
    this->close();
}

void AddBatchWidget::addBatchResultSlot(int result, QString fileName)
{
    ui->progressBar->setValue(++m_currentIndex);
    switch (result)
    {
        case 0:
        {
            m_successCount++;
            break;
        }
        case 1:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,The picture without faces!").arg(fileName));
            break;
        }
        case 2:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,The picture has multi-faces !").arg(fileName));
            break;
        }
        case 3:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,The face area is too small !").arg(fileName));
            break;
        }

        case 4:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,The Align score is too low!").arg(fileName));
            break;
        }
        case 5:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,The picture has a side face!").arg(fileName));
            break;
        }
        case 6:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,The face image is fuzzy !").arg(fileName));
            break;
        }
        case 7:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,The picture faces is too far!").arg(fileName));
            break;
        }
        case 8:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,init process failed!").arg(fileName));
            break;
        }
        case 9:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the function arg is invalid !").arg(fileName));
            break;
        }
        case 10:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the feature file is invalid !").arg(fileName));
            break;
        }
        case 11:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,HW_AUTH init failed!").arg(fileName));
            break;
        }
        case 12:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the LICENSE add failed!").arg(fileName));
            break;
        }
        case 13:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the id is not exist!").arg(fileName));
            break;
        }
        case 14:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,please take off your mask!").arg(fileName));
            break;
        }
        case 15:
        {
            ui->progressBar->setValue(selectFiles.count());
            ui->textEdit->append(QString("Picture %1 Add image failed,the Face database is full!").arg(fileName));
            break;
        }
        case 16:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the id is is invalid!!").arg(fileName));
            break;
        }
        case 17:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the files saved failed!").arg(fileName));
            break;
        }
        case 18:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the file is not jpg!").arg(fileName));
            break;
        }
        case 19:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the RPC op-failed!").arg(fileName));
            break;
        }
        case 20:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the unknown TAG!").arg(fileName));
            break;
        }
        case 160:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the AI-SERVICE not ready!").arg(fileName));
            break;
        }
        case -2:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the ID can not be empty!").arg(fileName));
            break;
        }
        case -3:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,ID length cannot exceed 32 bytes!").arg(fileName));
            break;
        }
        case -4:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the picture data is error!").arg(fileName));
            break;
        }
        case -10:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the feature file is to large,please input the picture below 4M!").arg(fileName));
            break;
        }
        case -11:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,please check the validity of the input file name!").arg(fileName));
            break;
        }
        case 1001:
        {
            ui->textEdit->append(QString("Picture %1 Add image failed,the SDK_PROXY not ready!").arg(fileName));
            break;
        }

        default:
        {
            ui->textEdit->append(QString("Picture %1 upload failed:Add image Timeout!").arg(fileName));

            break;
        }
    }

    if(m_currentIndex == selectFiles.count() || result == 15)
    {
        ui->textEdit->append(QString("Result:upload %1 files,%2 successes, %3 failures ").arg(selectFiles.count())
                             .arg(m_successCount).arg(selectFiles.count() - m_successCount));
        ui->okBtn->setEnabled(true);
        ui->selectBtn->setEnabled(true);
        ui->uploadBtn->setEnabled(true);
        selectFiles.clear();
        m_isCancel = true;
    }
    else
    {
        if(m_isCancel)
        {
            return;
        }

        MtcManager::instance()->addImageBatch(selectFiles.at(m_currentIndex));
    }
}
