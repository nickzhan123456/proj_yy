#ifndef SETRECOGNIZECOUNT_H
#define SETRECOGNIZECOUNT_H

#include <QDialog>

namespace Ui {
class setrecognizecount;
}

class setrecognizecount : public QDialog
{
    Q_OBJECT

public:
    explicit setrecognizecount(QWidget *parent = nullptr);
    ~setrecognizecount();

    void initWidget(QString title);
//    static bool getCount(QWidget *parent, const QString &title, int *recognizeCount,int *livingCount);

    int getRecognizeCount();
    int getLivingCount();
private slots:
    void acceptSlot();
signals:
    void getCountSignal(int recognizeCount,int livingCount);
private:
    Ui::setrecognizecount *ui;
};

#endif // SETRECOGNIZECOUNT_H
