// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_INCLUDES_H
#define CLARK_INCLUDES_H

#include <QObject>

#include <vector>

// $todo: refactor with symbolinfoobjectbase
class IncludesInFile : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool isComplete READ isComplete WRITE setComplete NOTIFY completeChanged)
public:
  explicit IncludesInFile(const QString& filePath, QObject* parent = nullptr);
  ~IncludesInFile();

  bool isComplete() const;
  void setComplete(bool c = true);

  const QString& filePath() const;

  struct Include
  {
    int line;
    QString included_file;
  };

  const std::vector<Include>& includesInFile() const;
  void setIncludesInFile(std::vector<Include> incls);
  void addIncludesInFile(const std::vector<Include>& incls);

  bool hasInclude(int line, QString* included_file = nullptr) const;
  QString getInclude(int line) const;

Q_SIGNALS:
  void completeChanged();
  void completed();
  void changed();

private:
  bool m_complete = false;
  QString m_filepath;
  std::vector<Include> m_includes;
};

#endif // CLARK_INCLUDES_H
