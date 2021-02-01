#pragma once

#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>

#include <TCPPacket.h>
#include <Logger.h>

#include <unordered_map>

class TCPServer : public QObject {
    Q_OBJECT

public:
    TCPServer(Logger& log, unsigned int port);

private:
    QTcpServer* server;
    unordered_map<QTcpSocket*, string> clientIPs;
    Logger& log;

private slots:
    void onAcceptConnection();
    void onReadyRead();
    void onDisconnected();

signals:
    void recieved(const string, TCPPacket);
    void connected(const string, unsigned int);
    void disconnected(const string);

};
