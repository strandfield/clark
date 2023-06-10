// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "tusymbol.h"
 
ClangSymbolObject::ClangSymbolObject(const libclang::Cursor& c) : 
  m_cursor(c)
{
  setName(c.getSpelling().c_str());
  setUsr(c.getUSR().c_str());
  setFullName(c.getDisplayName().c_str());

  setComplete();
}

ClangSymbolObject::~ClangSymbolObject()
{

}

const libclang::Cursor& ClangSymbolObject::cursor() const
{
  return m_cursor;
}
