#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class SettingWidget;
}

class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWidget(QWidget *parent = nullptr);
    ~SettingWidget();

private slots:
    void applySlots();

signals:
    void applyCamera(QString cameraName,QString serialPort);

private:
    void initCameraInfo();

private:
    Ui::SettingWidget *ui;
    QTimer m_timer;
};

#endif // SETTINGWIDGET_H
