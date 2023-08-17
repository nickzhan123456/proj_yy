#include "facerect.h"
#include "ui_facerect.h"
#include <QPainter>
#include <QDebug>

FaceRect::FaceRect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FaceRect)
{
    ui->setupUi(this);
    ui->nameLabel->setStyleSheet("QLabel { background-color : red; color : white; }");
}

FaceRect::~FaceRect()
{
    delete ui;
}

void FaceRect::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::red,3));
    QRectF rectangle(ui->rectWidget->pos().x(),ui->rectWidget->pos().y(),m_w,m_h);
//            QRect rectangle(QPoint(face.left*ration, face.top*ration), QPoint(face.right*ration, face.bottom*ration));
    painter.drawRect(rectangle);
}

void FaceRect::setName(QString name)
{
    ui->nameLabel->setText(name);
}

int FaceRect::getNameHight()
{
    qDebug() << "height " << ui->nameLabel->height() << "   " << ui->nameLabel->size().height();
    return ui->nameLabel->height();
}

void FaceRect::resetRect(int w, int h)
{
    if(qAbs(m_h-h) > 15 || qAbs(m_w-w) > 15)
    {
        m_h = h;
        m_w = w;
        ui->rectWidget->resize(w,h);
    }
}
