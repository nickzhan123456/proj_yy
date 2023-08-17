#ifndef UPGRADEWIDGET_H
#define UPGRADEWIDGET_H

#include <QWidget>

namespace Ui {
class UpgradeWidget;
}

class UpgradeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UpgradeWidget(QWidget *parent = nullptr);
    ~UpgradeWidget();

private slots:
    void selectFileSlot();
    void upgradeSlot();
    void cancelSlot();
    void updateUploadProgress(int value);
    void setUploadRange(int value);
    void uploadStatusSlots(bool isComplete,QString msg,bool isSuccess);

signals:
    void upgradeSuccess();

private:

protected:
    virtual void closeEvent(QCloseEvent* ev)override;
private:
    Ui::UpgradeWidget *ui;
    QString m_selectFile;
};

#endif // UPGRADEWIDGET_H
