#pragma once

#include <string>
#include <string.h>
#include <stdlib.h>

#include <util/strutil.h>
#include <util/strbuilder.h>
#include <tcp/TCPException.h>

class TCPPacket {

public:
    enum PacketType {
        TEXT, FILE
    };

    static TCPPacket decode(std::string packet);
    static std::string encode(PacketType type, std::string filename, std::string content);

    PacketType getType() {
        return type;
    }

    std::string getContent() {
        return content;
    }

    std::string getFilename() {
        return filename;
    }

    std::string getRaw() {
        return raw;
    }

    // for tests only
    TCPPacket(std::string raw) : raw(raw) {}

private:
    PacketType type;
    std::string filename;
    std::string content;
    std::string raw;

    TCPPacket& withType(PacketType type) {
        this->type = type;
        return *this;
    }

    TCPPacket& withContent(std::string content) {
        this->content = content;
        return *this;
    }

    TCPPacket& withFilename(std::string filename) {
        this->filename = filename;
        return *this;
    }

    static const char* getContentFromRaw(const char* cstr);
    static bool tryParseFilePacket(const char* cstr, char* filename);
    static bool tryParseTextPacket(const char* cstr);
};


