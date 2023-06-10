// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "tuincludesinfile.h"

#include <libclang-utils/findincludesinfile.h>

#include <QtConcurrent> 
#include <QFutureWatcher>

#include <QDebug>

std::vector<IncludesInFile::Include> find_includes_in_file(ClangIncludesInFile* includes)
{
  std::vector<IncludesInFile::Include> list;

  auto func = [&list](const libclang::Cursor& c, const libclang::SourceRange& source_range) {
    libclang::SpellingLocation loc = source_range.getRangeStart().getSpellingLocation();
    IncludesInFile::Include incl;
    incl.line = loc.line;
    incl.included_file = QString::fromStdString(c.getIncludedFile().getFileName());
    list.push_back(incl);
  };

  libclang::findIncludesInFile(includes->translationUnit(), includes->file(), func);

  return list;
}
 
ClangIncludesInFile::ClangIncludesInFile(const libclang::TranslationUnit& tu, const QString& filePath, const libclang::File& file, QObject* parent)
  : IncludesInFile(filePath, parent),
    m_translation_unit(tu),
    m_file(file)
{
  m_find_includes_future = QtConcurrent::run(&find_includes_in_file, this);

  auto* watcher = new QFutureWatcher<std::vector<Include>>(this);
  connect(watcher, &QFutureWatcher<std::vector<Include>>::finished, this, &ClangIncludesInFile::onFutureFinished);
  watcher->setFuture(m_find_includes_future);
}

ClangIncludesInFile::~ClangIncludesInFile()
{

}

const libclang::TranslationUnit& ClangIncludesInFile::translationUnit() const
{
  return m_translation_unit;
}

const libclang::File& ClangIncludesInFile::file() const
{
  return m_file;
}

void ClangIncludesInFile::onFutureFinished()
{
  setIncludesInFile(m_find_includes_future.result());
  setComplete();
}

