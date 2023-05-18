// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "settings.h"

#include <QStandardPaths>
#include <QVariant>

Settings::Settings(QObject* parent)
  : QSettings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/settings.ini", QSettings::IniFormat, parent)
{

}

Settings::~Settings()
{

}

bool Settings::readBool(const QString& key, bool defaultValue) const 
{
  QVariant result = value(key);
  return result.isNull() ? defaultValue : result.toBool();
}

void Settings::writeBool(const QString& key, bool val)
{
  QVariant result = value(key);

  if (result.isNull() || result.toBool() != val)
  {
    setValue(key, val);
    Q_EMIT valueChanged(key);
  }
}

int Settings::readInt(const QString& key, int defaultValue) const
{
  QVariant result = value(key);
  return result.isNull() ? defaultValue : result.toInt();
}

void Settings::writeInt(const QString& key, int val)
{
  QVariant result = value(key);

  if (result.isNull() || result.toInt() != val)
  {
    setValue(key, val);
    Q_EMIT valueChanged(key);
  }
}

QString Settings::readString(const QString& key, const QString& defaultValue) const
{
  QVariant result = value(key);
  return result.isNull() ? defaultValue : result.toString();
}

void Settings::writeString(const QString& key, const QString& val)
{
  QVariant result = value(key);

  if (result.isNull() || result.toString() != val)
  {
    setValue(key, val);
    Q_EMIT valueChanged(key);
  }
}
