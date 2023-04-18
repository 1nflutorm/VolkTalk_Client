#ifndef GROUPINFOFORM_H
#define GROUPINFOFORM_H

#include <QDialog>
#include <QMouseEvent>
#include <QVector>
#include <QString>

namespace Ui {
class GroupInfoForm;
}

class GroupInfoForm : public QDialog
{
    Q_OBJECT

public:
    explicit GroupInfoForm(QWidget *parent = nullptr);
    ~GroupInfoForm();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::GroupInfoForm *ui;
    QPoint m_pCursor;
    void addItemToLWidget(const QString& Name);

public slots:
    void showForm(const QString& GroupName, const quint16& MaxNumOfUsers, const quint16& currNumOfUsers, QVector<QString> CurrUsers);
};

#endif // GROUPINFOFORM_H
