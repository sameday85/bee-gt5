#include <QFile>
#include "urlconnection.h"

UrlConnection::UrlConnection()
{
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), this,
                SLOT(downloadFinished(QNetworkReply*)));
}

QNetworkReply *UrlConnection::doGet(const QNetworkRequest &request) {
    error = false;
    QNetworkReply *reply = manager.get(request);
    loop.exec();
    return reply;
}

bool UrlConnection::isError() {
    return error;
}

QNetworkReply *UrlConnection::doPost(const QNetworkRequest &request, const QByteArray &data) {
    error = false;
    QNetworkReply *reply = manager.post(request, data);
    loop.exec();
    return reply;
}

bool UrlConnection::download2File(QString url, QString filename) {
    QNetworkRequest request(url);
    QNetworkReply *reply = doGet(request);
    if (isError())
        return false;
    return saveToDisk(filename, reply);
}

QString UrlConnection::download2String(QString url) {
    QString response;

    QNetworkRequest request(url);
    QNetworkReply *reply = doGet(request);
    if (!isError())
        response = saveToString(reply);

    return response;
}

bool UrlConnection::isHttpRedirect(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303
            || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void UrlConnection::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    if (reply->error()) {
        error = true;
    }
    else if (isHttpRedirect(reply)) {
        error = true;
    }
    loop.exit();
}

bool UrlConnection::saveToDisk(const QString &filename, QNetworkReply *reply)
{
    bool ret = false;
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(reply->readAll());
        file.close();
        ret = true;
    }
    reply->deleteLater();

    return ret;
}

QString UrlConnection::saveToString(QNetworkReply *reply) {
    QString content = QString::fromUtf8(reply->readAll());
    reply->deleteLater();

    return content;
}
