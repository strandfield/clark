// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_SETTINGS_H
#define CLARK_SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
  Q_OBJECT

public:
  explicit Settings(QObject* parent = nullptr);
  ~Settings();

  template<typename T>
  T read(const QString& key, const T& default_value) const;

  template<typename T>
  void write(const QString& key, const T& val);

  bool readBool(const QString& key, bool defaultValue = false) const;
  void writeBool(const QString& key, bool val);

  int readInt(const QString& key, int defaultValue = 0) const;
  void writeInt(const QString& key, int val);

  QString readString(const QString& key, const QString& defaultValue = QString()) const;
  void writeString(const QString& key, const QString& val);

  static QString libclangPathKey();

Q_SIGNALS:
  void valueChanged(const QString& key);
};

template<typename T>
inline T Settings::read(const QString& key, const T& default_value) const
{
  QVariant result = value(key);
  return result.isNull() ? default_value : result.value<T>();
}

template<typename T>
inline void Settings::write(const QString& key, const T& val)
{
  QVariant result = value(key);

  if (result.isNull() || result.value<T>() != val)
  {
    setValue(key, val);
    Q_EMIT valueChanged(key);
  }
}

#endif // CLARK_SETTINGS_H
