#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QApplication>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QQueue>
#include <QUrl>
#include "dbmanager.h"

typedef DBManager::Reciter Reciter;

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr,
                             DBManager *dbptr = nullptr,
                             QList<Reciter> reciters = QList<Reciter>());
    struct DownloadTask
    {
        int surah;
        int verse;
        int reciterIdx;
        QUrl link;
        QNetworkReply *networkReply = nullptr;
        DownloadTask() {}
    };

    bool isDownloading() const;

    DownloadTask currentTask() const;

    QList<QString> reciterDirNames() const;

    QNetworkAccessManager *netMan() const;

    QList<Reciter> recitersList() const;

public slots:
    void startQueue();
    void stopQueue();
    void cancelCurrentTask();
    void enqeueVerseTask(int reciterIdx, int surah, int verse);
    void processQueueHead();
    void finishupTask(QNetworkReply *replyData);
    bool saveFile(QNetworkReply *data, QString filename);

signals:
    void downloadStarted();
    void downloadProgressed(int downloaded, int total);
    void downloadCanceled();
    void downloadComplete();
    void downloadError();
    void queueEmpty();

private:
    void handleConError(QNetworkReply::NetworkError err);
    bool m_isDownloading = false;
    QNetworkAccessManager *m_netMan;
    DBManager *m_dbPtr;
    DownloadTask m_currentTask;
    QQueue<DownloadTask> m_downloadQueue;
    const QList<Reciter> m_recitersList;
    QDir m_downloadPath;
};

#endif // DOWNLOADMANAGER_H
