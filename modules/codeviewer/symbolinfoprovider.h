// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_SYMBOLINFOPROVIDER_H
#define CLARK_SYMBOLINFOPROVIDER_H

#include "symbolinfoobject.h"

#include "token.h"

#include <QFlags>

class IncludesInFile;

class SymbolInfoProvider : public QObject
{
  Q_OBJECT
public:
  explicit SymbolInfoProvider(QObject* parent = nullptr);
  ~SymbolInfoProvider();

  enum Feature
  {
    None                 = 0,
    SymbolAtLocation     = 1,
    ReferencesInDocument = 2,
    IncludesInFile       = 4,
  };
  Q_ENUM(Feature)

  typedef QFlags<Feature> Features;

  virtual Features features() const;
  bool hasFeature(Feature f) const;

  virtual SymbolObject* getSymbol(const TokenInfo& tokinfo);
  virtual SymbolReferencesInDocument* getReferencesInDocument(SymbolObject* symbol, const QString& filePath);

  virtual ::IncludesInFile* getIncludesInFile(const QString& filePath);
};

#endif // CLARK_SYMBOLINFOPROVIDER_H
