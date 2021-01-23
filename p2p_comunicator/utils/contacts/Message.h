#pragma once

#include <string>

#include <QJsonObject>
#include <QDateTime>

#include <tcp/TCPPacket.h>
#include <config/Config.h>
#include <util/strbuilder.h>

using namespace std;

class Message {

public:
    enum Type {
        TEXT, FILE
    };

    Message(TCPPacket* packet);
    Message(QJsonObject& object);
    QJsonObject serialize();
    void downloadFile();
    string getTimestamp();

    string& getContent() {
        return content;
    }

    string& getFilename() {
        return filename;
    }

    Type getType() {
        return type;
    }

private:
    QDateTime timestamp;
    Type type;
    string content;
    string filename;

    Message(QDateTime timestamp, Type type, string content, string filename) : timestamp(timestamp), type(type), content(content), filename(filename) {}

};
