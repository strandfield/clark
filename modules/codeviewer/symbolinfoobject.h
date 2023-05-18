// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_SYMBOLINFOOBJECT_H
#define CLARK_SYMBOLINFOOBJECT_H

#include <QObject>
#include <QPointer>

#include <optional>
#include <vector>

class QTextDocument;

class SymbolObject : public QObject
{
  Q_OBJECT
public:
  explicit SymbolObject(QObject* parent = nullptr);
  ~SymbolObject();

  const QString& name() const;
  void setName(const QString& name);

  const QString& fullName() const;
  void setFullName(const QString& fullname);

  const QString& usr() const;
  void setUsr(const QString& usr);

  int id() const;
  void setId(int id);

  bool isSame(const SymbolObject& other) const;

private:
  QString m_name;
  QString m_fullname;
  QString m_usr;
  int m_id = -1;
};

class SymbolInfoObjectBase : public QObject
{
  Q_OBJECT
 
  Q_PROPERTY(bool isComplete READ isComplete WRITE setComplete NOTIFY completeChanged)
public:
  explicit SymbolInfoObjectBase(SymbolObject& sym, QObject* parent = nullptr);

  SymbolObject* symbol() const;

  bool isComplete() const;
  void setComplete(bool c = true);

Q_SIGNALS:
  void completeChanged();
  void completed();

private:
  QPointer<SymbolObject> m_symbol;
  bool m_complete = false;
};

class SymbolReferencesInDocument : public SymbolInfoObjectBase
{
  Q_OBJECT
public:
  explicit SymbolReferencesInDocument(SymbolObject& sym, const QString& filePath, QObject* parent = nullptr);

  const QString& filePath() const;

  struct Position
  {
    int line;
    int col;
  };

  const std::vector<Position>& referencesInFile() const;
  void setReferencesInFile(std::vector<Position> refs);
  void addReferencesInFile(const std::vector<Position>& refs);

Q_SIGNALS:
  void changed();

private:
  QString m_filepath;
  std::vector<Position> m_references;
};

#endif // CLARK_SYMBOLINFOOBJECT_H
