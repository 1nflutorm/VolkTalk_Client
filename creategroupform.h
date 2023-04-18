#ifndef CREATEGROUPFORM_H
#define CREATEGROUPFORM_H

#include <QDialog>
#include <QMouseEvent>
#include <QIntValidator>

namespace Ui {class CreateGroupForm;}

class CreateGroupForm : public QDialog
{
    Q_OBJECT

public:
    explicit CreateGroupForm(QWidget *parent = nullptr);
    ~CreateGroupForm();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::CreateGroupForm *ui;
    QPoint m_pCursor;
    QIntValidator *intValidator;

private slots:
    void createButton_clicked();

    void on_groupNameEdit_returnPressed();

public slots:
    void showForm();

signals:
    void signalCreateGroup(const QString& Name, const QString& numberOfUsers);

};

#endif // CREATEGROUPFORM_H
