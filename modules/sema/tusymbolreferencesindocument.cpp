// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "tusymbolreferencesindocument.h"

#include <libclang-utils/findreferencesinfile.h>

#include <QtConcurrent> 
#include <QFutureWatcher>

std::vector<SymbolReferencesInDocument::Position> find_references_in_file(ClangSymbolReferencesInDocument* references)
{
  std::vector<SymbolReferencesInDocument::Position> positions;

  auto func = [&positions](const libclang::Cursor& c, const libclang::SourceRange& source_range) {
    libclang::SpellingLocation loc = source_range.getRangeStart().getSpellingLocation();
    SymbolReferencesInDocument::Position pos;
    pos.line = loc.line;
    pos.col = loc.col;
    positions.push_back(pos);
  };

  libclang::findReferencesInFile(references->symbol()->cursor(), references->file(), func);

  return positions;
}
 
ClangSymbolReferencesInDocument::ClangSymbolReferencesInDocument(ClangSymbolObject& sym, const QString& filePath, const libclang::File& file, QObject* parent)
  : SymbolReferencesInDocument(sym, filePath, parent),
    m_file(file)
{
  m_find_references_future = QtConcurrent::run(&find_references_in_file, this);

  auto* watcher = new QFutureWatcher<std::vector<Position>>(this);
  connect(watcher, &QFutureWatcher<std::vector<Position>>::finished, this, &ClangSymbolReferencesInDocument::onFutureFinished);
  watcher->setFuture(m_find_references_future);
}

ClangSymbolReferencesInDocument::~ClangSymbolReferencesInDocument()
{

}

ClangSymbolObject* ClangSymbolReferencesInDocument::symbol() const
{
  return static_cast<ClangSymbolObject*>(SymbolReferencesInDocument::symbol());
}

const libclang::File& ClangSymbolReferencesInDocument::file() const
{
  return m_file;
}

void ClangSymbolReferencesInDocument::onFutureFinished()
{
  setReferencesInFile(m_find_references_future.result());
  setComplete();
}

