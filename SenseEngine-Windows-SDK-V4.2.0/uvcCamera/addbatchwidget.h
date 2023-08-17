#ifndef ADDBATCHWIDGET_H
#define ADDBATCHWIDGET_H

#include <QWidget>

namespace Ui {
class AddBatchWidget;
}

class AddBatchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddBatchWidget(QWidget *parent = nullptr);
    ~AddBatchWidget();

private slots:
    void selectFilesSlot();
    void uploadSlot();
    void cancelSlot();
    void addBatchResultSlot(int result,QString fileName);
protected:
    virtual void closeEvent(QCloseEvent *event) override;
private:
    Ui::AddBatchWidget *ui;
    QStringList selectFiles;
    int m_currentIndex = 0;
    int m_successCount = 0;
    bool m_isCancel = true;
};

#endif // ADDBATCHWIDGET_H
