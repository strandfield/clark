// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "tusymbolinfoprovider.h"

#include "tusymbol.h"
#include "tuincludesinfile.h"
#include "tusymbolreferencesindocument.h"

#include <libclang-utils/clang-cursor.h>
#include <libclang-utils/clang-source-location.h>
#include <libclang-utils/clang-translation-unit.h>

#include <QTextDocument>

#include <QDebug>

TranslationUnitSymbolInfoProvider::TranslationUnitSymbolInfoProvider(TranslationUnitHandle handle, const QTextDocument& document) :
  m_handle(handle)
{
  QString filepath = document.metaInformation(QTextDocument::DocumentUrl);
  const libclang::TranslationUnit& tu = m_handle.clangTranslationunit();
  m_file = std::make_unique<libclang::File>(tu.getFile(filepath.toStdString()));
}

TranslationUnitSymbolInfoProvider::~TranslationUnitSymbolInfoProvider()
{

}

TranslationUnitSymbolInfoProvider::Features TranslationUnitSymbolInfoProvider::features() const
{
  return { Feature::SymbolAtLocation, Feature::ReferencesInDocument };
}

SymbolObject* TranslationUnitSymbolInfoProvider::getSymbol(const TokenInfo& tokinfo)
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
    return new TranslationUnitSymbolObject(c);
  }
  catch (...) {
    return nullptr;
  }
}

SymbolReferencesInDocument* TranslationUnitSymbolInfoProvider::getReferencesInDocument(SymbolObject* symbol, const QString& filePath)
{
  auto* tusymbol = qobject_cast<TranslationUnitSymbolObject*>(symbol);

  if (!tusymbol)
    return nullptr;

  return new TranslationUnitSymbolReferencesInDocument(*tusymbol, filePath, *m_file);
}

::IncludesInFile* TranslationUnitSymbolInfoProvider::getIncludesInFile(const QString& filePath)
{
  return new TranslationUnitIncludesInFile(m_handle.clangTranslationunit(), filePath , *m_file);
}
