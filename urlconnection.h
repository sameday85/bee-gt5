#ifndef URLCONNECTION_H
#define URLCONNECTION_H

#include <QtCore>
#include <QtNetwork>

class UrlConnection : public QObject {
    Q_OBJECT
    QNetworkAccessManager manager;

public:
    UrlConnection();

    QNetworkReply * doGet(const QNetworkRequest &other);
    QNetworkReply * doPost(const QNetworkRequest &other, const QByteArray &data);

    bool isError();
    bool download2File(QString url, QString filename);
    QString download2String(QString url);
    QString saveToString(QNetworkReply *reply);

public slots:
    void downloadFinished(QNetworkReply *reply);
    //void sslErrors(const QList<QSslError> &errors);

private:
    bool saveToDisk(const QString &filename, QNetworkReply *reply);
    bool isHttpRedirect(QNetworkReply *reply);
private:
    QEventLoop loop;
    bool error;
};

#endif // URLCONNECTION_H
