// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_TUSYMBOL_H
#define CLARK_TUSYMBOL_H

#include <codeviewer/symbolinfoobject.h>

#include <libclang-utils/clang-cursor.h>

class TranslationUnitSymbolObject : public SymbolObject
{
  Q_OBJECT
public:
  explicit TranslationUnitSymbolObject(const libclang::Cursor& c);
  ~TranslationUnitSymbolObject();

  const libclang::Cursor& cursor() const;

private:
  libclang::Cursor m_cursor;
};

#endif // CLARK_TUSYMBOL_H
