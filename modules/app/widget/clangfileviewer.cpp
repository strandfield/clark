// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "clangfileviewer.h"

#include "sema/tunameresolver.h"
#include "sema/tusymbolinfoprovider.h"

#include <libclang-utils/clang-translation-unit.h>

/**
 * \brief constructs a file viewer
 * \param thandle  a valid handle to the translation unit
 * \param file     the file
 * \param parent   optional parent widget
 * 
 * The path and content of \a file are retrieved from the translation unit.
 */
ClangFileViewer::ClangFileViewer(const TranslationUnitHandle& thandle, const libclang::File& file, QWidget* parent) :
  CodeViewer(file.getFileName().c_str(), thandle.clangTranslationunit().getFileContents(file), parent),
  m_thandle(thandle),
  m_file(file)
{
  if (auto* highlighter = qobject_cast<CpptokSyntaxHighlighter*>(syntaxHighlighter()))
  {
    highlighter->setNameResolver(new TranslationUnitNameResolver(thandle, *document()));
  }

  setSymbolInfoProvider(new TranslationUnitSymbolInfoProvider(thandle, *document()));
}

const libclang::File& ClangFileViewer::file() const
{
  return m_file;
}
