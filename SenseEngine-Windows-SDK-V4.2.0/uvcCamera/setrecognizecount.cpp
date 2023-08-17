#include "setrecognizecount.h"
#include "ui_setrecognizecount.h"
#include <QMessageBox>
#include <QRegularExpressionValidator>

setrecognizecount::setrecognizecount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setrecognizecount)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
    connect(ui->okBtn,&QPushButton::clicked,this,&setrecognizecount::acceptSlot);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &setrecognizecount::reject);
}

setrecognizecount::~setrecognizecount()
{
    delete ui;
}


void setrecognizecount::initWidget(QString title)
{
    this->setWindowTitle(title);
}

//bool setrecognizecount::getCount(QWidget *parent, const QString &title, int *recognizeCount,int *livingCount)
//{
//    setrecognizecount setrecognizecountDialog(parent);
//    setrecognizecountDialog.initWidget(title);
//    if (setrecognizecountDialog.exec() == QDialog::Accepted) {
//        *recognizeCount = setrecognizecountDialog.getLivingCount();
//        *livingCount = setrecognizecountDialog.getLivingCount();
//        return true;
//    }
//    return false;
//}

int setrecognizecount::getRecognizeCount()
{
    return ui->spinBox->value();
}

int setrecognizecount::getLivingCount()
{
    return ui->spinBox_2->value();
}

void setrecognizecount::acceptSlot()
{
    emit getCountSignal(getRecognizeCount(),getLivingCount());
    emit accept();
}
