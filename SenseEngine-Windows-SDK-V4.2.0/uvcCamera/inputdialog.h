#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>

namespace Ui {
class InputDialog;
}

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputDialog(QWidget *parent = nullptr);
    ~InputDialog();

    void initWidget(QString title,QString lable);

    static QString getText(QWidget *parent, const QString &title, const QString &label,bool *ok = nullptr);

    QString getInput();

private slots:
    void acceptSlot();

private:
    Ui::InputDialog *ui;
};

#endif // INPUTDIALOG_H
