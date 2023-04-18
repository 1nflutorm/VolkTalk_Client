#include "clientwindow.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::ClientWindow)
{
    ui->setupUi(this);

    ui->label->setText("VolkTalk");

    ip = "192.168.50.23";

    ui->LoginEdit->setMaxLength(20);
    ui->registerLoginEdit->setMaxLength(20);
    ui->messageEdit->setMaxLength(150);

    ui->MainTabWidget->tabBar()->hide();
    ui->MainTabWidget->setCurrentIndex(0);
    ui->LoginEdit->setFocus();

    nextBlockSize = 0;
    connected = false;

    setMessengerDefault();

    ui->UsersWidget->setFocusPolicy(Qt::NoFocus);
    ui->RoomsWidget->setFocusPolicy(Qt::NoFocus);

    socket = new QTcpSocket(this);//создаю новый сокет
    connect(socket, &QTcpSocket::readyRead, this, &ClientWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &ClientWindow::deleteLater);

    QFontDatabase::addApplicationFont(":/fonts/Roboto-Medium.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Roboto-Thin.ttf");

    this->setAttribute(Qt::WA_TranslucentBackground);

    MsgBox = new MessageBox;
    MsgBox->setWindowFlags(Qt::FramelessWindowHint);

    GroupForm = new CreateGroupForm;
    GroupForm->setWindowFlags(Qt::FramelessWindowHint);

    GroupInfo = new GroupInfoForm;
    GroupInfo->setWindowFlags(Qt::FramelessWindowHint);

    UserInfo = new UserInfoForm;
    UserInfo->setWindowFlags(Qt::FramelessWindowHint);

    connect(ui->closeButton, &QPushButton::clicked, this, &ClientWindow::slotCloseWindow);
    connect(ui->hideButton, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->maximizeButton, &QPushButton::clicked, this, &QWidget::showMinimized);

    connect(ui->RegisterButton, &QPushButton::clicked, this, &ClientWindow::RegisterButton_clicked);
    connect(ui->LoginButton, &QPushButton::clicked, this, &ClientWindow::LoginButton_clicked);
    connect(ui->CancelLoginButton, &QPushButton::clicked, this, &ClientWindow::CancelLoginButton_clicked);

    connect(ui->registerCancelButton, &QPushButton::clicked, this, &ClientWindow::registerCancelButton_clicked);
    connect(ui->registerLoginButton, &QPushButton::clicked, this, &ClientWindow::registerLoginButton_clicked);

    connect(ui->writeToUserButton, &QPushButton::clicked, this, &ClientWindow::writeToUserButton_clicked);
    connect(ui->cancelWritingButton, &QPushButton::clicked, this, &ClientWindow::cancelWritingButton_clicked);

    connect(ui->sendButton, &QPushButton::clicked, this, &ClientWindow::sendButton_clicked);

    connect(ui->createRoomButton, &QPushButton::clicked, GroupForm, &CreateGroupForm::showForm);
    connect(GroupForm, &CreateGroupForm::signalCreateGroup, this, &ClientWindow::slotCreateGroup);
    connect(ui->connectToRoomButton, &QPushButton::clicked, this, &ClientWindow::connectToRoomButton_clicked);
    connect(ui->deleteRoomButton, &QPushButton::clicked, this, &ClientWindow::deleteRoomButton_clicked);
    connect(ui->cancelConnectToRoomButton, &QPushButton::clicked, this, &ClientWindow::cancelConnectToRoomButton_clicked);

    connect(this, &ClientWindow::signalErrorOccured, MsgBox, &MessageBox::slotErrorOccured);

    connect(ui->leaveButton, &QPushButton::clicked, this, &ClientWindow::leaveButton_clicked);

    connect(ui->RoomsWidget, &QListWidget::doubleClicked, this, &ClientWindow::RoomItemDoubleclicked);
    connect(this, &ClientWindow::signalShowGroupInfo, GroupInfo, &GroupInfoForm::showForm);

    connect(ui->UsersWidget, &QListWidget::doubleClicked, this, &ClientWindow::UserItemDoubleclicked);
    connect(this, &ClientWindow::signalShowUserForm, UserInfo, &UserInfoForm::showForm);

    connect(ui->registerRepeatPasswordEdit, &QLineEdit::returnPressed, this, &ClientWindow::registerLoginButton_clicked);
    connect(ui->PasswordEdit,  &QLineEdit::returnPressed, this, &ClientWindow::LoginButton_clicked);
    connect(ui->messageEdit, &QLineEdit::returnPressed, this, &ClientWindow::sendButton_clicked);

}

ClientWindow::~ClientWindow()
{
    delete ui;
}

QString ClientWindow::checkCorrectRegistrationData()
{
    QString name = ui->nameEdit->text();
    QString surname = ui->surnameEdit->text();
    QString lastname = ui->lastnameEdit->text();
    QString login = ui->registerLoginEdit->text();
    QString password = ui->registerPasswordEdit->text();
    QString repPassword = ui->registerRepeatPasswordEdit->text();

    if(surname.length() < 2)
        return "Фамилия не может быть такой короткой!";

    if(name.length() < 2)
        return "Имя не может быть таким коротким!";

    if(lastname.length() < 2 && lastname.length() > 0)
        return "Отчество не может быть таким коротким!";

    if(login.length() < 2)
        return "Логин не может быть таким коротким!";

    if(password.length() < 2)
        return "Пароль не может быть таким коротким!";

    if (password != repPassword)
        return "Пароли не совпадают!";

    return "correct";
}

void ClientWindow::sendRegistragionRequestToServer()
{
    if(!connected)
    {
        socket->connectToHost(ip, 2323);
        if(!socket->waitForConnected(3000))
        {
            emit signalErrorOccured("Ошибка подключения к серверу!");
            return;
        }
        connected = true;
    }
    QString name = ui->nameEdit->text();
    QString surname = ui->surnameEdit->text();
    QString lastname = ui->lastnameEdit->text();
    QString Password = ui->registerPasswordEdit->text();
    m_Login = ui->registerLoginEdit->text();
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out.device()->seek(0);
    out << quint16(0) << quint16(REGISTRATION) << m_Login << Password << surname << name << lastname << QTime::currentTime();
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}

void ClientWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_4);
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    if(in.status() == QDataStream::Ok)
    {
        while(true)
        {
            if(nextBlockSize == 0)
            {
                if(socket->bytesAvailable() < 2)
                    break;
                in >> nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize)
                break;


            quint16 operation;
            QString exeption;
            QString tempLogin, msg, RoomName;
            quint16 size;
            in >> operation;
            switch (operation)
            {
            case LOG_IN:
            {
                Data.clear();
                out.device()->seek(0);
                out << quint16(0) <<  quint16(CURRENT_STATE) << m_Login;
                out.device()->seek(0);
                out << quint16(Data.size() - sizeof(quint16));
                socket->write(Data);
                break;
            }
            case CURRENT_STATE:
            {
                in >> size;
                for(int i = 0; i < size; i++)
                {
                    in >> tempLogin;
                    addItemToListWidget(tempLogin, ui->UsersWidget, QColor(0, 160, 0));

                }
                in >> size;
                for(int i = 0; i < size; i++)
                {
                    in >> tempLogin;
                    addItemToListWidget(tempLogin, ui->UsersWidget, QColor(174, 173, 172));
                }
                in >> size;
                for(int i = 0; i < size; i++)
                {
                    quint16 maxNumOfUsers, NumOfUsers;
                    in >> RoomName >> NumOfUsers >> maxNumOfUsers;
                    QString title = RoomName + "   " + QString::number(NumOfUsers) + "/" + QString::number(maxNumOfUsers);
                    addItemToListWidget(title, ui->RoomsWidget, QColor(0, 160, 0));
                }
                ui->UsersWidget->sortItems(Qt::AscendingOrder);
                ui->MainTabWidget->setCurrentIndex(2);
                break;
            }
            case CLIENT_CONNECTED:
            {
                in >> tempLogin;
                setOnline(tempLogin);
                break;
            }
            case CLIENT_DISCONNECTED:
            {
                in >> tempLogin;
                setOffline(tempLogin);
                break;
            }
            case APPEND_MESSAGE_HISTORY:
            {
                in >> size;
                for(int i = 0; i < size; i++)
                {
                    in >> msg;
                    ui->MessageBrowser->append(msg);
                }
                break;
            }
            case GET_INFO:
            {
                quint16 type;
                in >> type;
                if(type == GROUP_CHAT)
                {
                    QVector<QString> namesVec;
                    quint16 maxNumOfUsers, currNumOfUsers, size;
                    QString RoomName, name;

                    in >> RoomName >> maxNumOfUsers >> currNumOfUsers >> size;
                    for(int i = 0; i < size; i++)
                    {
                        in >> name;
                        namesVec.push_back(name);
                    }
                    emit signalShowGroupInfo(RoomName, maxNumOfUsers, currNumOfUsers, namesVec);
                    break;
                }
                if(type == PRIVATE_CHAT)
                {
                    QString Login, id, surname, name, lastname;
                    in >> Login >> id >> surname >> name >> lastname;

                    emit signalShowUserForm(id, surname, name, lastname, Login);
                    break;
                }
                break;
            }
            case PRIVATE_CHAT:
            {
                in >> tempLogin >> msg;
                if(tempLogin == CurrentChatName)
                {
                    ui->MessageBrowser->append(msg);
                }
                break;
            }
            case GROUP_CHAT:
            {
                QString GroupName;
                in >> GroupName >> msg;
                if(GroupName == CurrentChatName)
                {
                    ui->MessageBrowser->append(msg);
                }
                break;
            }
            case CREATE_GROP_CHAT:
            {
                QString GroupName;
                quint16 curNumOfUsers, maxNumOfUsers;
                in >> GroupName >> curNumOfUsers >> maxNumOfUsers;

                addGroup(GroupName, curNumOfUsers, maxNumOfUsers);

                break;
            }
            case DELETE_GROUP_CHAT:
            {
                QString GroupName, msg;
                in >> GroupName >> msg;

                if(CurrentChatName == GroupName)
                {
                    setMessengerDefault();
                    ui->MessageBrowser->append(msg);
                }

                for(int i = 0; i < ui->RoomsWidget->count(); i++)
                {
                    QString currentName = ui->RoomsWidget->item(i)->text();
                    if(currentName.startsWith(GroupName))
                    {
                        QListWidgetItem* item = ui->RoomsWidget->takeItem(i);
                        delete item;
                    }
                }

                break;
            }
            case CLIENT_CONNECTED_TO_CHAT:
            {
                QString GroupName, tempLogin;
                quint16 maxNumOfUsers, NumOfUsers;
                in >> tempLogin >> GroupName >> NumOfUsers >> maxNumOfUsers;

                updateNumOfUsers(GroupName, NumOfUsers, maxNumOfUsers);

                if(tempLogin == m_Login)
                {
                    setMessengerDefault();

                    CurrentChatName = GroupName;
                    CurrentMessageType = GROUP_CHAT;

                    ui->currentChatLabel->setText("Комната: " + CurrentChatName);

                    Data.clear();
                    QDataStream out(&Data, QIODevice::WriteOnly);
                    out.setVersion(QDataStream::Qt_6_4);
                    out.device()->seek(0);
                    out << quint16(0) << quint16(APPEND_MESSAGE_HISTORY) << quint16(GROUP_CHAT) << CurrentChatName;
                    out.device()->seek(0);
                    out << quint16(Data.size() - sizeof(quint16));
                    socket->write(Data);
                    ui->messageEdit->setFocus();
                }

                break;
            }
            case CLIENT_DISCONNECTED_FROM_CHAT:
            {
                QString GroupName, tempLogin;
                quint16 maxNumOfUsers, NumOfUsers;
                in >> tempLogin >> GroupName >> NumOfUsers >> maxNumOfUsers;

                updateNumOfUsers(GroupName, NumOfUsers, maxNumOfUsers);

                break;
            }
            case EXEPTION:
            {
                in >> exeption;
                emit signalErrorOccured(exeption);
                break;
            }
            default:
                break;
            }
            nextBlockSize = 0;
        }

    }
}

void ClientWindow::RegisterButton_clicked()
{
    ui->MainTabWidget->setCurrentIndex(1);
    ui->surnameEdit->setFocus();
}

void ClientWindow::LoginButton_clicked()
{
    if(!connected)
    {
        socket->connectToHost(ip, 2323);
        if(!socket->waitForConnected(10000))
        {
            emit signalErrorOccured("Ошибка подключения к серверу!");
            return;
        }
        connected = true;
    }
    m_Login = ui->LoginEdit->text();
    QString Password = ui->PasswordEdit->text();
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out.device()->seek(0);
    out << quint16(0) << quint16(LOG_IN) << m_Login << Password << QTime::currentTime();
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}

void ClientWindow::CancelLoginButton_clicked()
{
    ui->LoginEdit->clear();
    ui->PasswordEdit->clear();
}

void ClientWindow::registerCancelButton_clicked()
{
    ui->MainTabWidget->setCurrentIndex(0);
    ui->nameEdit->clear();
    ui->surnameEdit->clear();
    ui->lastnameEdit->clear();
    ui->registerLoginEdit->clear();
    ui->registerPasswordEdit->clear();
    ui->registerRepeatPasswordEdit->clear();
}

void ClientWindow::registerLoginButton_clicked()
{
    QString errorMsg = checkCorrectRegistrationData();
    if(errorMsg == "correct")
    {
        sendRegistragionRequestToServer();
        return;
    }
    emit signalErrorOccured(errorMsg);
}

void ClientWindow::writeToUserButton_clicked()
{
    if(CurrentMessageType == GROUP_CHAT)
    {
        sendDisconnectFromChatRequest();
    }
    QListWidgetItem *item = ui->UsersWidget->currentItem();
    if(!item)
        return;

    setMessengerDefault();

    CurrentChatName = item->text();
    CurrentMessageType = PRIVATE_CHAT;

    ui->currentChatLabel->setText("Собеседник: " + CurrentChatName);

    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out.device()->seek(0);
    out << quint16(0) << quint16(APPEND_MESSAGE_HISTORY) << quint16(PRIVATE_CHAT) << m_Login << CurrentChatName;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);

    ui->UsersWidget->clearSelection();
    ui->messageEdit->setFocus();
}

void ClientWindow::sendButton_clicked()
{
    if(CurrentChatName == "" && CurrentMessageType == NONE_TYPE)
    {
        ui->messageEdit->clear();
        return;
    }

    QString msg = ui->messageEdit->text();
    if(msg == "")
        return;

    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out.device()->seek(0);
    out << quint16(0) << quint16(CurrentMessageType) << m_Login << CurrentChatName << msg;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);

    ui->messageEdit->clear();
    ui->messageEdit->setFocus();
}

void ClientWindow::deleteRoomButton_clicked()
{
    QString RoomName;
    QListWidgetItem *item = ui->RoomsWidget->currentItem();

    if(!item)
        return;

    RoomName = item->text();

    QString NewStr = "";
    int pos = RoomName.lastIndexOf("   ");
    for(int i = 0; i < pos; i++)
    {
        NewStr += RoomName[i];
    }
    RoomName = NewStr;

    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out.device()->seek(0);
    out << quint16(0) << quint16(DELETE_GROUP_CHAT) << m_Login << RoomName;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);

    ui->RoomsWidget->clearSelection();
}

void ClientWindow::connectToRoomButton_clicked()
{
    if(!ui->RoomsWidget->currentItem())
        return;

    if(CurrentMessageType == GROUP_CHAT)
    {
        sendDisconnectFromChatRequest();
    }
    QString RoomName = ui->RoomsWidget->currentItem()->text();

    RoomName = cutRoomName(RoomName);

    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out.device()->seek(0);
    out << quint16(0) << quint16(CLIENT_CONNECTED_TO_CHAT) << m_Login << RoomName;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);

    ui->messageEdit->setFocus();
    ui->RoomsWidget->clearSelection();
}

void ClientWindow::leaveButton_clicked()
{
    if(CurrentMessageType == GROUP_CHAT)
        sendDisconnectFromChatRequest();


    setMessengerDefault();
}

void ClientWindow::cancelWritingButton_clicked()
{
    ui->UsersWidget->clearSelection();
}

void ClientWindow::RoomItemDoubleclicked()
{
    QString RoomName = ui->RoomsWidget->currentItem()->text();

    RoomName = cutRoomName(RoomName);

    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out.device()->seek(0);
    out << quint16(0) << quint16(GET_INFO) << quint16(GROUP_CHAT) << RoomName;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}

void ClientWindow::UserItemDoubleclicked()
{
    QString Login = ui->UsersWidget->currentItem()->text();

    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out.device()->seek(0);
    out << quint16(0) << quint16(GET_INFO) << quint16(PRIVATE_CHAT) << Login;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}

void ClientWindow::cancelConnectToRoomButton_clicked()
{
    ui->RoomsWidget->clearSelection();
}

void ClientWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_pCursor = event->globalPos() - this->geometry().topLeft();
        event->accept();
    }
}

void ClientWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        this->move(event->globalPos() - m_pCursor);
        event->accept();
    }
}

void ClientWindow::on_LoginEdit_returnPressed()
{
    ui->PasswordEdit->setFocus();
}

void ClientWindow::on_nameEdit_returnPressed()
{
    ui->lastnameEdit->setFocus();
}

void ClientWindow::on_surnameEdit_returnPressed()
{
    ui->nameEdit->setFocus();
}

void ClientWindow::on_lastnameEdit_returnPressed()
{
    ui->registerLoginEdit->setFocus();
}

void ClientWindow::on_registerLoginEdit_returnPressed()
{
    ui->registerPasswordEdit->setFocus();
}

void ClientWindow::on_registerPasswordEdit_returnPressed()
{
    ui->registerRepeatPasswordEdit->setFocus();
}

void ClientWindow::setOnline(const QString Login)
{
    for(int i = 0; i < ui->UsersWidget->count(); i++)
    {
        QListWidgetItem *item = ui->UsersWidget->item(i);
        if(item->text() == Login)
        {
            item->setForeground(QColor(0, 160, 0));
            return;
        }
    }
    QListWidgetItem *item = new QListWidgetItem(ui->UsersWidget);
    item->setText(Login);
    item->setForeground(QColor(0, 160, 0));
    ui->UsersWidget->sortItems(Qt::AscendingOrder);
}

void ClientWindow::setOffline(const QString Login)
{
    for(int i = 0; i < ui->UsersWidget->count(); i++)
    {
        QListWidgetItem *item = ui->UsersWidget->item(i);
        if(item->text() == Login)
        {
            item->setForeground(Qt::lightGray);
        }
    }
}

void ClientWindow::addGroup(const QString& ChatName, const quint16& curNumOfUsers, const quint16& maxNumOfUsers)
{
    QListWidgetItem *item = new QListWidgetItem(ui->RoomsWidget);
    QString Name = ChatName + "   " + QString::number(curNumOfUsers) + "/" + QString::number(maxNumOfUsers);
    item->setText(Name);
    item->setForeground(QColor(0, 160, 0));
    ui->UsersWidget->sortItems(Qt::AscendingOrder);
}

void ClientWindow::setMessengerDefault()
{
    ui->MessageBrowser->clear();
    CurrentMessageType = NONE_TYPE;
    CurrentChatName = "";
    ui->currentChatLabel->setText("Нет собеседника");
}

void ClientWindow::updateNumOfUsers(const QString& ChatName, const quint16& NumOfUsers, const quint16& maxNumOfUsers)
{
    for(int i = 0; i < ui->RoomsWidget->count(); i++)
    {
        QListWidgetItem *item = ui->RoomsWidget->item(i);
        QString temp = item->text();
        if(temp.startsWith(ChatName))
        {
            QString Name = ChatName + "   " + QString::number(NumOfUsers) + "/" + QString::number(maxNumOfUsers);
            item->setText(Name);
            return;
        }
    }
}

void ClientWindow::addItemToListWidget(const QString &title, QListWidget *LWidget, QColor color)
{
    QListWidgetItem *item = new QListWidgetItem(LWidget);
    item->setText(title);
    item->setForeground(color);
}

void ClientWindow::sendDisconnectFromChatRequest()
{
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    Data.clear();
    out.device()->seek(0);
    out << quint16(0) << quint16(CLIENT_DISCONNECTED_FROM_CHAT) << m_Login;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}

QString ClientWindow::cutRoomName(const QString &RoomName)
{
    QString NewStr = "";
    int pos = RoomName.lastIndexOf("   ");
    for(int i = 0; i < pos; i++)
    {
        NewStr += RoomName[i];
    }
    return NewStr;
}

void ClientWindow::slotCloseWindow()
{
    if(connected)
    {
        socket->disconnectFromHost();
    }
    this->close();
}

void ClientWindow::slotCreateGroup(const QString &Name, const QString &numberOfUsers)
{
    bool ok = false;
    int numberOfusers = numberOfUsers.toInt(&ok);
    if(Name.length() < 2)
    {
        emit signalErrorOccured("Имя комнаты не может быть таким коротким!");
        return;
    }
    if(numberOfusers < 1 || !ok)
    {
        emit signalErrorOccured("Некорректное количнство пользователей!");
        return;
    }

    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    Data.clear();
    out.device()->seek(0);
    out << quint16(0) << quint16(CREATE_GROP_CHAT) << m_Login << Name << quint16(numberOfusers);
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
    GroupForm->close();
}
