#include "inputdialog.h"
#include <QMessageBox>
#include "ui_inputdialog.h"
#include <QRegularExpressionValidator>

InputDialog::InputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
    connect(ui->okBtn,&QPushButton::clicked,this,&InputDialog::acceptSlot);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &InputDialog::reject);
}

InputDialog::~InputDialog()
{
    delete ui;
}

void InputDialog::initWidget(QString title, QString lable)
{
    this->setWindowTitle(title);
    ui->label->setText(lable);
}

QString InputDialog::getText(QWidget *parent, const QString &title, const QString &label, bool *ok)
{
    InputDialog inputDialog(parent);
    inputDialog.initWidget(title,label);
    if (inputDialog.exec() == QDialog::Accepted) {
        *ok = true;
        return inputDialog.getInput();
    }
    *ok = false;
    return "";
}

QString InputDialog::getInput()
{
    return ui->lineEdit->text();
}

void InputDialog::acceptSlot()
{
    if(ui->lineEdit->text().toUtf8().length() >32)
    {
        QMessageBox::warning(this,this->windowTitle(),"Beyond the input limit, please re-enter! \
                                                      No more than 11 characters in Chinese and 32 characters in English!");
    }else
    {
        emit accept();
    }
}
