// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "symbolinfoprovider.h"

SymbolInfoProvider::SymbolInfoProvider(QObject* parent) : QObject(parent)
{

}

SymbolInfoProvider::~SymbolInfoProvider()
{

}

SymbolInfoProvider::Features SymbolInfoProvider::features() const
{
  return Feature::None;
}

bool SymbolInfoProvider::hasFeature(Feature f) const
{
  return features().testFlag(f);
}

SymbolObject* SymbolInfoProvider::getSymbol(const TokenInfo& /* tokinfo */)
{
  return nullptr;
}

SymbolReferencesInDocument* SymbolInfoProvider::getReferencesInDocument(SymbolObject* /* symbol */, const QString& /* filePath */)
{
  return nullptr;
}

::IncludesInFile* SymbolInfoProvider::getIncludesInFile(const QString& /* filePath */)
{
  return nullptr;
}
