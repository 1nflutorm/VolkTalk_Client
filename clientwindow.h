#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QFontDatabase>
#include <QTabBar>
#include <QMouseEvent>
#include <QTcpSocket>
#include <QDataStream>
#include <QTime>
#include <QPainter>
#include <QBoxLayout>
#include <QThread>
#include <QMap>
#include <QListWidget>
#include "messagebox.h"
#include "creategroupform.h"
#include "groupinfoform.h"
#include "userinfoform.h"

enum functions
{
    REGISTRATION = 21,
    LOG_IN,
    CLIENT_CONNECTED,
    CLIENT_DISCONNECTED,

    CURRENT_STATE,
    APPEND_MESSAGE_HISTORY,
    GET_INFO,

    PRIVATE_CHAT,
    GROUP_CHAT,
    NONE_TYPE,

    CREATE_GROP_CHAT,
    DELETE_GROUP_CHAT,
    CLIENT_CONNECTED_TO_CHAT,
    CLIENT_DISCONNECTED_FROM_CHAT,
    EXEPTION,
    DISCONNECTED
};

QT_BEGIN_NAMESPACE
namespace Ui { class ClientWindow; }
QT_END_NAMESPACE

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

private:
    Ui::ClientWindow *ui;
    QPoint m_pCursor;

    MessageBox* MsgBox;

    CreateGroupForm* GroupForm;

    GroupInfoForm* GroupInfo;
    UserInfoForm* UserInfo;

    QString ip;
    QString CurrentChatName;

    quint16 CurrentMessageType;
    quint16 nextBlockSize;

    QTcpSocket* socket;

    QByteArray Data;

    QString m_Login;

    bool connected;

    void sendRegistragionRequestToServer();
    void setOnline(const QString Login);
    void setOffline(const QString Login);
    void addGroup(const QString& ChatName, const quint16& curNumOfUsers, const quint16& maxNumOfUsers);
    void setMessengerDefault();
    void updateNumOfUsers(const QString& ChatName, const quint16& NumOfUsers, const quint16& maxNumOfUsers);
    void addItemToListWidget(const QString& title, QListWidget* LWidget, QColor color);
    void sendDisconnectFromChatRequest();
    QString checkCorrectRegistrationData();
    QString cutRoomName(const QString& RoomName);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void slotReadyRead();
    void RegisterButton_clicked();
    void LoginButton_clicked();
    void CancelLoginButton_clicked();
    void registerCancelButton_clicked();
    void registerLoginButton_clicked();
    void writeToUserButton_clicked();
    void sendButton_clicked();
    void deleteRoomButton_clicked();
    void connectToRoomButton_clicked();
    void leaveButton_clicked();
    void cancelWritingButton_clicked();
    void RoomItemDoubleclicked();
    void UserItemDoubleclicked();
    void cancelConnectToRoomButton_clicked();
    void on_LoginEdit_returnPressed();
    void on_nameEdit_returnPressed();
    void on_surnameEdit_returnPressed();
    void on_lastnameEdit_returnPressed();
    void on_registerLoginEdit_returnPressed();
    void on_registerPasswordEdit_returnPressed();
    void slotCloseWindow();

public slots:
    void slotCreateGroup(const QString& Name, const QString& numberOfUsers);

signals:
    void signalErrorOccured(const QString errorMsg);
    void signalShowGroupInfo(const QString& GroupName, const quint16& MaxNumOfUsers, const quint16& currNumOfUsers, QVector<QString> CurrUsers);
    void signalShowUserForm(const QString& id, const QString& surname, const QString& name, const QString& lastname, const QString& login);
};
#endif // CLIENTWINDOW_H
