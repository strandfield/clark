// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_INDEXER_H
#define CLARK_INDEXER_H

#include "indexingresult.h"

#include "program/translationunit.h"

#include <libclang-utils/clang-index.h>

#include <QObject>

namespace clark
{

IndexingResult index_translation_unit(libclang::Index& index, libclang::TranslationUnit& tunit);

const char* get_file_contents(const libclang::TranslationUnit& tunit, const File& file);

} // namespace clark

class TranslationUnitIndexing : public QObject
{
  Q_OBJECT
public:
  explicit TranslationUnitIndexing(TranslationUnit& tunit, QObject* parent = nullptr);

  enum State
  {
    Init,
    Started,
    Ready,
  };

  State state() const;
  bool isStarted() const;
  bool isReady() const;
  
  TranslationUnit& translationUnit() const;

  void start();

  const clark::IndexingResult& indexingResult() const;
  void setIndexingResult(clark::IndexingResult r);

Q_SIGNALS:
  void started();
  void ready();

private:
  TranslationUnit& m_translation_unit;
  State m_state = Init;
  clark::IndexingResult m_result;
};

#endif // CLARK_INDEXER_H
