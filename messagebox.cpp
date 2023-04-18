#include "messagebox.h"
#include "ui_messagebox.h"

MessageBox::MessageBox(QWidget *parent) : QDialog(parent), ui(new Ui::MessageBox)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);
    connect(ui->closeButton, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->OkButton, &QPushButton::clicked, this, &QWidget::close);
    ui->picture_label->setStyleSheet("image: url(:/MessageBoxPict/critical.png);");
}

MessageBox::~MessageBox()
{
    delete ui;
}

void MessageBox::slotErrorOccured(const QString errorMsg)
{
    ui->Title_label->setText("Ошибка");
    ui->information_label->setText(errorMsg);
    ui->picture_label->setStyleSheet("image: url(:/MessageBoxPict/critical.png);");
    this->setModal(true);
    this->show();
}

void MessageBox::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_pCursor = event->globalPos() - this->geometry().topLeft();
        event->accept();
    }
}

void MessageBox::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        this->move(event->globalPos() - m_pCursor);
        event->accept();
    }
}
