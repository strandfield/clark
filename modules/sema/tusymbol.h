// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_TUSYMBOL_H
#define CLARK_TUSYMBOL_H

#include <codeviewer/symbolinfoobject.h>

#include <libclang-utils/clang-cursor.h>

class ClangSymbolObject : public SymbolObject
{
  Q_OBJECT
public:
  explicit ClangSymbolObject(const libclang::Cursor& c);
  ~ClangSymbolObject();

  const libclang::Cursor& cursor() const;

private:
  libclang::Cursor m_cursor;
};

#endif // CLARK_TUSYMBOL_H
