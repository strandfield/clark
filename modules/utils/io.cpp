// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "io.h"

#include <QFile>

namespace clark
{

namespace io
{

/**
 * \brief returns whether a file exists on disk
 * \param path  the filepath
 */
bool exists(const QString& path)
{
  return QFile::exists(path);
}

/**
 * \brief reads the content of a file on disk
 * \param path  the filepath
 */
QByteArray read_from_disk(const QString& path)
{
  QFile io{ path };
  io.open(QIODevice::ReadOnly);
  return io.readAll();
}

} // namespace io

} // namespace clark
