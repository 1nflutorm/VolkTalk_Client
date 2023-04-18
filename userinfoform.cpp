#include "userinfoform.h"
#include "ui_userinfoform.h"

UserInfoForm::UserInfoForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserInfoForm)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);

    connect(ui->closeButton, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->closeButton_2, &QPushButton::clicked, this, &QWidget::close);
}

UserInfoForm::~UserInfoForm()
{
    delete ui;
}

void UserInfoForm::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_pCursor = event->globalPos() - this->geometry().topLeft();
        event->accept();
    }
}

void UserInfoForm::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        this->move(event->globalPos() - m_pCursor);
        event->accept();
    }
}

void UserInfoForm::showForm(const QString &id, const QString &surname, const QString &name, const QString &lastname, const QString &login)
{
    ui->Title_label->setText("Информация");
    ui->User_label->setText("Пользователь: " + login);
    ui->Surname_label->setText("Фамилия: " + surname);
    ui->Name_label->setText("Имя: " + name);
    ui->LastName_label->setText("Отчество: " + lastname);
    ui->id_label->setText("id: " + id);

    this->setModal(true);
    this->show();

}

