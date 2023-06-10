// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "symbolinfoobject.h"

#include <QThread>

SymbolObject::SymbolObject(QObject* parent) : SemaObject(parent)
{

}

SymbolObject::~SymbolObject()
{

}

const QString& SymbolObject::name() const
{
  return m_name;
}

void SymbolObject::setName(const QString& name)
{
  m_name = name;
}

const QString& SymbolObject::fullName() const
{
  return m_fullname;
}

void SymbolObject::setFullName(const QString& fullname)
{
  m_fullname = fullname;
}

const QString& SymbolObject::usr() const
{
  return m_usr;
}

void SymbolObject::setUsr(const QString& usr)
{
  m_usr = usr;
}

int SymbolObject::id() const
{
  return m_id;
}

void SymbolObject::setId(int id)
{
  m_id = id;
}

bool SymbolObject::isSame(const SymbolObject& other) const
{
  return name() == other.name()
    && usr() == other.usr()
    && id() == other.id();
}


SymbolInfoObjectBase::SymbolInfoObjectBase(SymbolObject& sym, QObject* parent) : SemaObject(parent),
  m_symbol(&sym)
{

}

SymbolObject* SymbolInfoObjectBase::symbol() const
{
  return m_symbol.data();
}


SymbolReferencesInDocument::SymbolReferencesInDocument(SymbolObject& sym, const QString& filePath, QObject* parent) : SymbolInfoObjectBase(sym, parent),
  m_filepath(filePath)
{

}

const QString& SymbolReferencesInDocument::filePath() const
{
  return m_filepath;
}

const std::vector<SymbolReferencesInDocument::Position>& SymbolReferencesInDocument::referencesInFile() const
{
  return m_references;
}

void SymbolReferencesInDocument::setReferencesInFile(std::vector<Position> refs)
{
  m_references = std::move(refs);
  Q_EMIT changed();
}

void SymbolReferencesInDocument::addReferencesInFile(const std::vector<Position>& refs)
{
  if (refs.empty())
    return;

  m_references.insert(m_references.end(), refs.begin(), refs.end());
  Q_EMIT changed();
}
