#include "MessageListDelegate.h"

MessageListDelegate::MessageListDelegate(QObject* parent)
    : QStyledItemDelegate(parent),
      padding(10,10),
      btnState(QStyle::State_Raised)
{
}

QSize MessageListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{

    if(!index.data().canConvert<Message*>())
        return QSize();

    Message* message = index.data().value<Message*>();
    auto contact = dynamic_cast<const Contact*>(index.model());

    QString contactName = message->getSender() == Message::CONTACT
            ? QString::fromStdString(contact->getName())
            : "Me";

    contactName.append(':');

    int height = getSenderHeight(contactName, option) + 2*padding.height();

    if(message->getType() == Message::TEXT){
        height += getMessageHeight(message, option);
        height += option.fontMetrics.lineSpacing();
    }else if(message->getType() == Message::FILE){
        height += getDownloadHeight(message, option);
        height += option.fontMetrics.lineSpacing();
    }

    return QSize(option.rect.width(), height);
}

void MessageListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)const {

    if(!index.data().canConvert<Message*>())
        return;

    Message* message = qvariant_cast<Message*>(index.data());
    auto contact = dynamic_cast<const Contact*>(index.model());

    QString sender = message->getSender() == Message::CONTACT
            ? QString::fromStdString(contact->getName()) : "Me";
    sender.append(':');

    // set styles
    QStyleOptionViewItem opt = option;
    const QWidget* widget = opt.widget;
    QStyle* style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem,&opt,painter,widget);

    //draw Message

    paintSender(sender,painter,option);
    paintTimeStamp(message,painter,option);
    paintMessage(message,painter, opt);

    if(message->getType() == Message::FILE)
    {
        // paint download button if message have file
        paintDownload(message, painter, opt);
    }

}

void MessageListDelegate::paintSender(const QString sender, QPainter* painter, const QStyleOptionViewItem &option) const {

    painter->save();

    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);

    QRect textRect = option.rect;
    textRect.setX(option.rect.left() + padding.width());
    textRect.setY(option.rect.top() + padding.height());
    textRect.setWidth(textRect.width()/2 - padding.width());

    painter->drawText(textRect,Qt::TextWordWrap, sender);

    painter->restore();
}

void MessageListDelegate::paintMessage(const Message* message, QPainter* painter, const QStyleOptionViewItem &option) const {
    int lineSpace = option.fontMetrics.lineSpacing();
    QRect textRect = option.rect;

    textRect.setX(option.rect.left() + padding.width());
    textRect.setY(option.rect.top() + padding.height());
    textRect.setWidth(textRect.width() - padding.width());

    if (message->getFilename() == "") {
        QString content = QString::fromStdString(*message->getContent());

        // print message
        textRect.setY(textRect.y() + 2*lineSpace);
        painter->drawText(textRect,Qt::TextWrapAnywhere, content);
    }
}

void MessageListDelegate::paintDownload(const Message* message, QPainter* painter, const QStyleOptionViewItem &option) const {

    int w = 25;
    int h = 25;
    int x = option.rect.left() + padding.width();
    int y = option.rect.bottom() - h - padding.height();

    QString fileName = QString::fromStdString(message->getFilename());
    QRect fileNameLabel = QRect(x,y,option.rect.width(),h);
    QImage img(":/Icons/icons/download_btn.png");
    QRect btnRect(x,y,w,h);

    if(message->getSender() == Message::ME){
        painter->drawText(
                    fileNameLabel,
                    Qt::TextWrapAnywhere,
                    "File '" + fileName + "' was sent."
                    );
    }else{
        if (!message->isSaved()) {
            QRect fileNameLabel = QRect(x+w+10,y+5,option.rect.width()-w,h);
            painter->drawText(fileNameLabel,Qt::TextWrapAnywhere,QString::fromStdString(message->getFilename()));
            painter->drawImage(btnRect,img,img.rect());
        } else {
            painter->drawText(
                    fileNameLabel,
                    Qt::TextWrapAnywhere,
                    "File '" + fileName + "' was saved."
                    );
        }
    }
}

void MessageListDelegate::paintTimeStamp(const Message *message, QPainter *painter, const QStyleOptionViewItem &option) const {

    painter->save();

    QFont font = painter->font();
    font.setItalic(true);
    painter->setFont(font);

    QString timestamp = QString::fromStdString(message->getTimestamp());

    QRect r = option.rect;
    QRect boundRect = option.fontMetrics.boundingRect(r, Qt::TextWrapAnywhere, timestamp);

    int x = r.left() + r.width() - padding.width()  - boundRect.width();
    int y = r.top() + padding.width();

    QRect textRect(x,y,boundRect.width(),boundRect.height());

    painter->drawText(textRect,Qt::TextWrapAnywhere, timestamp);

    painter->restore();
}

int MessageListDelegate::getMessageHeight(const Message* message, const QStyleOptionViewItem &option) const {

    QRect textRect = option.rect;
    textRect.setWidth(textRect.width() - 2*padding.width());
    QString content = "";
    if (message->getType() == Message::Type::TEXT) {
        content = QString::fromStdString(*message->getContent());
    }
    QRect contentRect = option.fontMetrics.boundingRect(textRect, Qt::TextWrapAnywhere,content);

    return contentRect.height();
}

int MessageListDelegate::getDownloadHeight(const Message* message, const QStyleOptionViewItem &option) const{
    QString fileName = QString::fromStdString(message->getFilename());

    if(message->getSender() == Message::ME){
        QRect fileNameRect = option.fontMetrics.boundingRect(
                    QRect(0,0,option.rect.width() - 2*padding.width(), 25),
                    Qt::TextWrapAnywhere,
                    "File '" + fileName + "' was sent."
                    );
        return fileNameRect.height();
    }

    // Message from Contact
    if(message->isSaved()){
        fileName = "File '" + fileName + "' was saved.";
    }

    QRect fileNameRect = option.fontMetrics.boundingRect(
                QRect(0,0,option.rect.width() - 35 - 2*padding.width(), 25),
                Qt::TextWrapAnywhere,
                fileName
                );

    if(fileNameRect.size().height() < 25 && !message->isSaved()){
        return 25;
    }else{
        return fileNameRect.height();
    }
}

int MessageListDelegate::getSenderHeight(const QString& contactName, const QStyleOptionViewItem &option) const{
    QRect textRect = option.rect;
    textRect.setWidth(textRect.width() - 2*padding.width());

    QRect senderRect = option.fontMetrics.boundingRect(textRect,Qt::TextWrapAnywhere,contactName);

    return senderRect.height();
}

bool MessageListDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem &option, const QModelIndex &index){

    if(!index.data().canConvert<Message*>())
        return false;

    Message* message = qvariant_cast<Message*>(index.data());
    if(message->getType() != Message::FILE || message->getSender() == Message::ME || message->isSaved()){
        // ignore click
        return false;
    }

    int w = 25;
    int h = 25;
    int x = option.rect.left() + padding.width();
    int y = option.rect.bottom() - h - padding.height();

    QRect btnRect = QRect(x,y,w,h);

    if(event->type() == QEvent::MouseButtonRelease ||
       event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
        QPoint clickPoint = mouseEvent->pos();
        // button clicked
        if(btnRect.contains(clickPoint)) message->save();

    }else{
        return false;
    }

    return true;
}










