// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_ICONCACHE_H
#define CLARK_ICONCACHE_H

#include <QObject>

#include <QIcon>

#include <map>

class IconCache : public QObject
{
  Q_OBJECT

public:
  explicit IconCache(QObject* parent = nullptr);
  ~IconCache();

  const QIcon& icon(const QString& name) const;

private:
  std::map<QString, QIcon> m_icons;
};

#endif // CLARK_ICONCACHE_H
