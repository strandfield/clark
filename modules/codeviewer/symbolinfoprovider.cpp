// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "symbolinfoprovider.h"

SemaInfoProvider::SemaInfoProvider(QObject* parent) : QObject(parent)
{

}

SemaInfoProvider::~SemaInfoProvider()
{

}

SemaInfoProvider::Features SemaInfoProvider::features() const
{
  return Feature::None;
}

bool SemaInfoProvider::hasFeature(Feature f) const
{
  return features().testFlag(f);
}

SymbolObject* SemaInfoProvider::getSymbol(const TokenInfo& /* tokinfo */)
{
  return nullptr;
}

SymbolReferencesInDocument* SemaInfoProvider::getReferencesInDocument(SymbolObject* /* symbol */, const QString& /* filePath */)
{
  return nullptr;
}

::IncludesInFile* SemaInfoProvider::getIncludesInFile(const QString& /* filePath */)
{
  return nullptr;
}
