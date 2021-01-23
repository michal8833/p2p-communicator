#include "Storage.h"


bool Storage::load() {
    QFile loadFile(QString(Config::get("history-log-file")));

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    read(loadDoc.object());

    return true;
}

bool Storage::save() const {
    QFile saveFile(QString(Config::get("history-log-file")));

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonObject contactsObject;
    write(contactsObject);
    QJsonDocument saveDoc(contactsObject);
    saveFile.write(saveDoc.toJson());

    return true;
}

void Storage::read(const QJsonObject &json) {

    contacts.clear();
    QJsonArray contactsArray = json["contacts"].toArray();
    for (int i = 0; i < contactsArray.size(); ++i) {
        QJsonObject contactObject = contactsArray[i].toObject();
        Contact* contact = new Contact("example", "127.0.0.1", 8080);
        contact->read(contactObject);
        contacts.insert(contact->getAddress(), contact);
    }
}

QMap<QString, Contact*> Storage::getContacts() {
    return contacts;
}

void Storage::write(QJsonObject &json) const {

    QJsonArray contactsArray;
    foreach (const Contact* contact, contacts.values()) {
        QJsonObject contactObject;
        contact->write(contactObject);
        contactsArray.append(contactObject);
    }
    json["contacts"] = contactsArray;
}

void Storage::addContact(Contact* newContact) {
    contacts.insert(newContact->getAddress(), newContact);
    save();
}

