#ifndef FACERECT_H
#define FACERECT_H

#include <QWidget>

namespace Ui {
class FaceRect;
}

class FaceRect : public QWidget
{
    Q_OBJECT

public:
    explicit FaceRect(QWidget *parent = nullptr);
    ~FaceRect();

    void setName(QString name);
    int getNameHight();
    void resetRect(int w,int h);

protected:
    void paintEvent(QPaintEvent *e);

private:
    Ui::FaceRect *ui;
    int m_h = 0;
    int m_w = 0;
};

#endif // FACERECT_H
