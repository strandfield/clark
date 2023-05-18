// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_CLANGINDEX_H
#define CLARK_CLANGINDEX_H

#include "utils/qmethod.h"

#include <program/translationunit.h>

#include <libclang-utils/clang-index.h>

#include <QObject>

#include <list>
#include <mutex>

class LibClang;
class Project;

class QRunnable;
class QThreadPool;

class ClangIndex;

class TranslationUnitLoaderFactory
{
public:
  virtual ~TranslationUnitLoaderFactory();

  virtual QRunnable* createLoader(ClangIndex& index, TranslationUnit& t);
};

class ClangIndex : public QObject
{
  Q_OBJECT
public:
  ClangIndex(LibClang& lib, QObject* parent = nullptr);
  ~ClangIndex();

  libclang::Index& libclangIndex() const;

  void addTranslationUnits(const std::vector<TranslationUnit*>& list, const program::CompileOptions& options);

  const std::vector<TranslationUnit*>& translationUnits() const;
  void setTranslationUnits(std::vector<TranslationUnit*> list);

  void load(TranslationUnit* tu);

  TranslationUnitLoaderFactory& loaderFactory() const;
  void setLoaderFactory(std::unique_ptr<TranslationUnitLoaderFactory> factory);

Q_SIGNALS:
  void translationUnitsAdded(int n);
  void translationUnitLoaded(TranslationUnit* tu);

protected:
  void addToList(TranslationUnit* tu);
  void manage(TranslationUnit* tu);
  void scheduleParsing(TranslationUnit* tu);
  Q_INVOKABLE void checkParsing();
  void parse(std::list<TranslationUnit*>::iterator it);
  void parse(TranslationUnit* tu);
  void checkUsed(TranslationUnit* tu);
  Q_INVOKABLE void unloadTranslationUnits();

private Q_SLOTS:
  void onTranslationUnitParsed();
  void onTranslationUnitUsedChanged();

private:
  LibClang& m_library;
  std::unique_ptr<libclang::Index> m_index;
  std::unique_ptr<TranslationUnitLoaderFactory> m_loader_factory;
  QThreadPool* m_thread_pool = nullptr;
  std::vector<TranslationUnit*> m_translation_units;
  std::mutex m_mutex;
  std::list<TranslationUnit*> m_translation_unit_parsing_queue;
  QMethod m_check_parsing;
  std::vector<TranslationUnit*> m_translation_units_to_unload;
  QMethod m_unload_translation_units;
};

#endif // CLARK_CLANGINDEX_H
