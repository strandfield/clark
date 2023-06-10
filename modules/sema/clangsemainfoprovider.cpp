// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "clangsemainfoprovider.h"

#include "clangsymbol.h"
#include "clangincludesinfile.h"
#include "clangsymbolreferencesindocument.h"

#include <libclang-utils/clang-cursor.h>
#include <libclang-utils/clang-source-location.h>
#include <libclang-utils/clang-translation-unit.h>

#include <QTextDocument>

#include <QDebug>

ClangSemaInfoProvider::ClangSemaInfoProvider(TranslationUnitHandle handle, const QTextDocument& document) :
  m_handle(handle)
{
  QString filepath = document.metaInformation(QTextDocument::DocumentUrl);
  const libclang::TranslationUnit& tu = m_handle.clangTranslationunit();
  m_file = std::make_unique<libclang::File>(tu.getFile(filepath.toStdString()));
}

ClangSemaInfoProvider::~ClangSemaInfoProvider()
{

}

ClangSemaInfoProvider::Features ClangSemaInfoProvider::features() const
{
  return { Feature::SymbolAtLocation, Feature::ReferencesInDocument, Feature::IncludesInFile };
}

SymbolObject* ClangSemaInfoProvider::getSymbol(const TokenInfo& tokinfo)
{
  const libclang::TranslationUnit& tu = m_handle.clangTranslationunit();
  libclang::SourceLocation loc = tu.getLocation(*m_file, tokinfo.line, tokinfo.column);
  libclang::Cursor c = m_handle.clangTranslationunit().getCursor(loc);

  if (c.isNull() || c.kind() == CXCursor_FirstInvalid)
    return nullptr;

  if (c.isReference())
    c = c.getReference();

  c = c.getCanonical();

  try 
  {
    return new ClangSymbolObject(c);
  }
  catch (...) {
    return nullptr;
  }
}

SymbolReferencesInDocument* ClangSemaInfoProvider::getReferencesInDocument(SymbolObject* symbol, const QString& filePath)
{
  auto* tusymbol = qobject_cast<ClangSymbolObject*>(symbol);

  if (!tusymbol)
    return nullptr;

  return new ClangSymbolReferencesInDocument(*tusymbol, filePath, *m_file);
}

::IncludesInFile* ClangSemaInfoProvider::getIncludesInFile(const QString& filePath)
{
  return new ClangIncludesInFile(m_handle.clangTranslationunit(), filePath , *m_file);
}
