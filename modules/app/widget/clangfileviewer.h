// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_CLANGFILEVIEWER_H
#define CLARK_CLANGFILEVIEWER_H

#include "codeviewer/codeviewer.h"

#include "program/translationunit.h"

#include <libclang-utils/clang-file.h>

/**
 * \brief a code viewer for libclang files
 */
class ClangFileViewer : public CodeViewer
{
  Q_OBJECT
public:
  ClangFileViewer(const TranslationUnitHandle& thandle, const libclang::File& file, QWidget* parent = nullptr);
  
  const libclang::File& file() const;

  static void setup(CodeViewer* viewer, const TranslationUnitHandle& thandle, const libclang::File& file);

private:
  TranslationUnitHandle m_thandle;
  libclang::File m_file;
};

#endif // CLARK_CLANGFILEVIEWER_H
