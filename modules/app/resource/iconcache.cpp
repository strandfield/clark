// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "iconcache.h"

#include <QDirIterator>

IconCache::IconCache(QObject* parent) : QObject(parent)
{
  QDirIterator iterator{ ":/media", QStringList() << "*.svg", QDir::Filter::Files, QDirIterator::IteratorFlag::Subdirectories };

  while (iterator.hasNext())
  {
    QString path = iterator.next();
    QString name = QFileInfo(path).baseName();
    m_icons[name] = QIcon(path);
  }
}

IconCache::~IconCache()
{

}

const QIcon& IconCache::icon(const QString& name) const
{
  static const QIcon default_icon = {};
  auto it = m_icons.find(name);
  return it == m_icons.end() ? default_icon : it->second;
}
