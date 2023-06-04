// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_LIBCLANG_H
#define CLARK_LIBCLANG_H

#include <libclang-utils/libclang.h>

#include <QObject>

class LibClang : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool libclangAvailable READ libclangAvailable NOTIFY libclangAvailableChanged)
public:
  explicit LibClang(QObject* parent = nullptr);
  explicit LibClang(const QString& libclangPath, QObject* parent = nullptr);
  ~LibClang();

  void setLibrary(std::shared_ptr<libclang::LibClang> lib);
  std::shared_ptr<libclang::LibClang> library() const;

  std::shared_ptr<libclang::LibClang> libclang() const;
  
  bool libclangAvailable() const;

  bool tryLoadLibclang(const QString& path = QString());

  static std::shared_ptr<libclang::LibClang> tryLoad(const QString& path = QString());

Q_SIGNALS:
  void libclangAvailableChanged();

private:
  std::shared_ptr<libclang::LibClang> m_libclang_library;
};

inline std::shared_ptr<libclang::LibClang> LibClang::libclang() const
{
  return library();
}

#endif // CLARK_LIBCLANG_H
