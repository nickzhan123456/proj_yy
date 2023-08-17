#ifndef ANTIFLICKERDIALOG_H
#define ANTIFLICKERDIALOG_H
#include <QButtonGroup>
#include <QDialog>

namespace Ui {
class AntiFlickerDialog;
}

class AntiFlickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AntiFlickerDialog(QWidget *parent = nullptr);
    ~AntiFlickerDialog();
    void setAntiFlicker(int cameraId, int enable, int frequency);

public slots:
    void acceptSlot();
    void rejectedSlot();

private:
    Ui::AntiFlickerDialog *ui;
    QButtonGroup *m_enableGroup = nullptr;
    QButtonGroup *m_frequencyGroup = nullptr;
    int m_cameraId;
};

#endif // ANTIFLICKERDIALOG_H
