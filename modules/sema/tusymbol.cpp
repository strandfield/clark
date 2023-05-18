// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "tusymbol.h"
 
TranslationUnitSymbolObject::TranslationUnitSymbolObject(const libclang::Cursor& c) : 
  m_cursor(c)
{
  setName(c.getSpelling().c_str());
  setUsr(c.getUSR().c_str());
  setFullName(c.getDisplayName().c_str());
}

TranslationUnitSymbolObject::~TranslationUnitSymbolObject()
{

}

const libclang::Cursor& TranslationUnitSymbolObject::cursor() const
{
  return m_cursor;
}
