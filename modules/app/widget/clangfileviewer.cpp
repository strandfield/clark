// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "clangfileviewer.h"

#include "sema/clangsyntaxhighlighter.h"
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
  setup(this, thandle, file);
}

const libclang::File& ClangFileViewer::file() const
{
  return m_file;
}

/**
 * \brief install a syntax highlighter and symbol info provider on the codeviewer
 */
void ClangFileViewer::setup(CodeViewer* viewer, const TranslationUnitHandle& thandle, const libclang::File& file)
{
  viewer->setSyntaxHighlighter(new ClangSyntaxHighlighter(thandle, file, viewer->document()));
  viewer->setSymbolInfoProvider(new TranslationUnitSymbolInfoProvider(thandle, *viewer->document()));
}
