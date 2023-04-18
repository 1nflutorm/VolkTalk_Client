#ifndef USERINFOFORM_H
#define USERINFOFORM_H

#include <QDialog>
#include <QMouseEvent>
#include <QString>

namespace Ui {
class UserInfoForm;
}

class UserInfoForm : public QDialog
{
    Q_OBJECT

public:
    explicit UserInfoForm(QWidget *parent = nullptr);
    ~UserInfoForm();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::UserInfoForm *ui;
    QPoint m_pCursor;

public slots:
    void showForm(const QString& id, const QString& surname, const QString& name, const QString& lastname, const QString& login);
};

#endif // USERINFOFORM_H
