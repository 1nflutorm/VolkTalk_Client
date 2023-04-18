#include "groupinfoform.h"
#include "ui_groupinfoform.h"

GroupInfoForm::GroupInfoForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GroupInfoForm)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);

    ui->UsersWidget->setFocusPolicy(Qt::NoFocus);

    connect(ui->closeButton, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->closeButton2, &QPushButton::clicked, this, &QWidget::close);

}

GroupInfoForm::~GroupInfoForm()
{
    delete ui;
}

void GroupInfoForm::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_pCursor = event->globalPos() - this->geometry().topLeft();
        event->accept();
    }
}

void GroupInfoForm::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        this->move(event->globalPos() - m_pCursor);
        event->accept();
    }
}

void GroupInfoForm::addItemToLWidget(const QString &Name)
{
    QListWidgetItem *item = new QListWidgetItem(ui->UsersWidget);
    item->setText(Name);
    item->setForeground(Qt::white);
}

void GroupInfoForm::showForm(const QString &GroupName, const quint16 &MaxNumOfUsers, const quint16 &currNumOfUsers, QVector<QString> CurrUsers)
{
    ui->Title_label->setText("Информация");
    ui->UsersWidget->clear();
    ui->GroupName_label->setText("Название комнаты: " + GroupName);
    ui->MaxNumOfUsers_label->setText("Максимальное количество пользователей: " + QString::number(MaxNumOfUsers));
    ui->CurrNumOfUsers_label->setText("Текущее количество пользователей: " + QString::number(currNumOfUsers));
    for(auto user : CurrUsers)
    {
        addItemToLWidget(user);
    }
    this->setModal(true);
    this->show();
}
