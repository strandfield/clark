// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_UTILS_IO_H
#define CLARK_UTILS_IO_H

#include <QByteArray>
#include <QString>

namespace clark
{

namespace io
{

QByteArray read_from_disk(const QString& path);

} // namespace io

} // namespace clark

#endif // CLARK_UTILS_IO_H
