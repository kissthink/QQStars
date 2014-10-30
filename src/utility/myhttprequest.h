#ifndef MYHTTPREQUEST_H
#define MYHTTPREQUEST_H

#include <QObject>
#include <QtNetwork>
#include <QQueue>
#include <QJSValue>

class NetworkAccessManager;
class MyHttpRequest : public QObject
{
    Q_OBJECT
    Q_PROPERTY( RequestStatus status READ status WRITE setStatus NOTIFY statusChanged)
    Q_ENUMS(RequestStatus)
    Q_ENUMS(Priority)
public:
    explicit MyHttpRequest(QObject *parent = 0);
    enum RequestStatus{
        Idle,//初始状态
        Busy//请求中
    };

    const NetworkAccessManager *getNetworkAccessManager();
    QNetworkRequest *getNetworkRequest();
protected:
    NetworkAccessManager *manager;
    QNetworkRequest request;
    QPointer<QNetworkReply> m_reply;
    
    RequestStatus m_status;
    RequestStatus status();
    void setStatus( RequestStatus new_status );
    
    enum ReplyType{//回调的几种方式
        CallbackFun,
        ConnectSlot
    };
    struct Data{
        ReplyType replyType;
        QJSValue callbackFun;
        QObject* caller;
        QByteArray slotName;
    };
    struct requestData{
        QUrl url;
        QByteArray data;
    };

    QQueue<Data> queue_replyData;
    QQueue<requestData> queue_requestData;
    
    void send(QObject *caller, QByteArray slotName, QUrl url, QByteArray data="", bool highRequest=false );//highRequest记录是否为高级的网络请求
    void send( QJSValue callbackFun, QUrl url, QByteArray data="", bool highRequest=false );
protected slots:
    virtual void finished( QNetworkReply *reply );
    void send();
signals:
    void statusChanged();
public slots:
    void get(QObject *caller, QByteArray slotName, QUrl url, bool highRequest=false );
    void post(QObject *caller, QByteArray slotName, QUrl url, QByteArray data="", bool highRequest=false );
    void get(QJSValue callbackFun, QUrl url, bool highRequest=false );
    void post(QJSValue callbackFun, QUrl url, QByteArray data="", bool highRequest=false );
    void abort();//取消当前网络请求
    QString errorString();
};

class MyHttpRequestPrivate : public MyHttpRequest
{
    Q_OBJECT
private:
    explicit MyHttpRequestPrivate(QNetworkRequest request, QJSValue callbackFun, QUrl url, QByteArray data);
    explicit MyHttpRequestPrivate(QNetworkRequest request, QObject *caller, QByteArray slotName, QUrl url, QByteArray data);
    friend class MyHttpRequest;
private slots:
    void finished( QNetworkReply *reply );
};

#endif // MYHTTPREQUEST_H
