#include "creategroupform.h"
#include "ui_creategroupform.h"

CreateGroupForm::CreateGroupForm(QWidget *parent) : QDialog(parent), ui(new Ui::CreateGroupForm)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);
    connect(ui->closeButton, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QWidget::close);

    ui->groupNameEdit->setMaxLength(20);

    ui->cancelButton->setAutoDefault(false);
    ui->closeButton->setAutoDefault(false);
    ui->createButton->setAutoDefault(false);

    connect(ui->groupNameEdit, &QLineEdit::returnPressed, this, &CreateGroupForm::on_groupNameEdit_returnPressed);
    connect(ui->createButton, &QPushButton::clicked, this, &CreateGroupForm::createButton_clicked);
    connect(ui->numberOfUsersEdit, &QLineEdit::returnPressed, this, &CreateGroupForm::createButton_clicked);

    intValidator = new QIntValidator(1, 99);
    ui->numberOfUsersEdit->setValidator(intValidator);
}

CreateGroupForm::~CreateGroupForm()
{
    delete ui;
}

void CreateGroupForm::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_pCursor = event->globalPos() - this->geometry().topLeft();
        event->accept();
    }
}

void CreateGroupForm::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        this->move(event->globalPos() - m_pCursor);
        event->accept();
    }
}

void CreateGroupForm::createButton_clicked()
{
    QString GroupName = ui->groupNameEdit->text();
    QString numberOfUsers = ui->numberOfUsersEdit->text();

    emit signalCreateGroup(GroupName, numberOfUsers);
}

void CreateGroupForm::showForm()
{
    ui->Title_label->setText("Добавление группы");
    ui->groupNameEdit->clear();
    ui->numberOfUsersEdit->clear();
    ui->groupNameEdit->setFocus();

    this->setModal(true);
    this->show();
}

void CreateGroupForm::on_groupNameEdit_returnPressed()
{
    ui->numberOfUsersEdit->setFocus();
}

