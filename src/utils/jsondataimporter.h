#ifndef JSONDATAIMPORTER_H
#define JSONDATAIMPORTER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <database/bookmarksdb.h>
#include <interfaces/userdataimporter.h>
#include <types/verse.h>

class JsonDataImporter : public UserDataImporter
{
public:
  JsonDataImporter();
  void importBookmarks() override;
  void importKhatmah() override;
  void importThoughts() override;
  void setFile(QString path) override;
  bool fileContains(QString key) override;
  bool read() override;

private:
  QSharedPointer<BookmarksDb> m_bookmarksDb = BookmarksDb::current();
  bool validArray(const QString key);
  bool validVerse(const QJsonObject& obj);
  bool validKhatmah(const QJsonObject& obj);
  bool validThought(const QJsonObject& obj);
  Verse verseFromJson(const QJsonObject& obj);
  QPair<QString, Verse> khatmahFromJson(const QJsonObject& obj);
  QPair<Verse, QString> thoughtFromJson(const QJsonObject& obj);
  QFileInfo m_file;
  QJsonObject m_fileObj;
};

#endif // JSONDATAIMPORTER_H
