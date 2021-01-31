#include <TCPConnection.h>

void TCPConnection::startServer(unsigned int port) {
    server = new TCPServer(log, port);
    this->myPort = port;

    connect(server, SIGNAL(connected(const string, unsigned int)), SIGNAL(connected(const string, unsigned int)));
    connect(server, SIGNAL(disconnected(const string)), SIGNAL(disconnected(const string)));
    connect(server, SIGNAL(recieved(const string, TCPPacket)), SIGNAL(recieved(const string, TCPPacket)));
}

TCPClient* TCPConnection::registerClient(const string ip, unsigned int port) {
    TCPClient* client = new TCPClient(log, ip, myPort, port);

    clients[ip] = client;
    connect(client, SIGNAL(failed(const string, TCPException)), SIGNAL(sendingError(const string, TCPException)));
    connect(client, SIGNAL(connected(const string, unsigned int)), SIGNAL(connected(const string, unsigned int)));
    connect(client, SIGNAL(disconnected(const string)), SIGNAL(disconnected(const string)));

    return client;
}

bool TCPConnection::isClientConnected(const string& ip) {
    return clients[ip]->isConnected();
}

void TCPConnection::closeConnection(const string& ip) {
    /* TODO: client gets disconnected but server is still connected,
     *       the next message recieved from the contact will not be
     *       registered as a new contact request, the message won't be
     *       saved and the app will crash
     *
     * SOLUTION 1: send a "disconnect" request to the contact, which will make
     *             them close their client -> server connection
     * */

    TCPClient* client = clients[ip];
    client->forceDisconnect();
    client->deleteLater();
    clients.erase(ip);
}

void TCPConnection::reconnect(const string& ip) {
    clients[ip]->tryConnect();
}

void TCPConnection::send(const string& ip, const string& content) {
    clients[ip]->send(content);
}
