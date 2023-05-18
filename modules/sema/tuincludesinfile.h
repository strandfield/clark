// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_TUINCLUDESINFILE_H
#define CLARK_TUINCLUDESINFILE_H

#include "codeviewer/includes.h"

#include <libclang-utils/clang-file.h>
#include <libclang-utils/clang-translation-unit.h>

#include <QFuture>

class TranslationUnitIncludesInFile : public IncludesInFile
{
  Q_OBJECT
public:
  explicit TranslationUnitIncludesInFile(const libclang::TranslationUnit& tu, const QString& filePath, const libclang::File& file, QObject* parent = nullptr);
  ~TranslationUnitIncludesInFile();

  const libclang::TranslationUnit& translationUnit() const;
  const libclang::File& file() const;

protected Q_SLOTS:
  void onFutureFinished();

private:
  const libclang::TranslationUnit& m_translation_unit;
  libclang::File m_file;
  QFuture<std::vector<Include>> m_find_includes_future;
};

#endif // CLARK_TUINCLUDESINFILE_H
