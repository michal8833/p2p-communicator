#pragma once
#include <string>

#include <QTcpSocket>
#include <QAbstractSocket>
#include <QHostAddress>
#include <QObject>

#include <contacts/Storage.h>
#include <config/Config.h>

using namespace std;

class TCPClient : public QObject {

    Q_OBJECT
public:
    TCPClient(Contact* contact);

    ~TCPClient() { delete socket; }

    void send(string& packet);

private:
    Contact* contact;
    QTcpSocket* socket;

private slots:
    void onDisconnect();
    void onConnect();

signals:
    void failed(Contact* contact, TCPException e);
    void connected(Contact* contact);
    void disconnected(Contact* contact);

};
