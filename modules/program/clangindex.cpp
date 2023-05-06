// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "clangindex.h"

#include "libclang.h"

#include <libclang-utils/clang-index.h>
#include <libclang-utils/clang-translation-unit.h>

#include <QThreadPool>

#include <QDebug>

#include <iostream>
#include <mutex>

class ParseTranslationUnit : public QRunnable
{
private:
  ClangIndex& m_index;
  TranslationUnit& m_translation_unit;

public:

  explicit ParseTranslationUnit(ClangIndex& index, TranslationUnit& tu) :
    m_index(index),
    m_translation_unit(tu)
  {
    setAutoDelete(true);
  }

  void run() override
  {
    m_translation_unit.setState(TranslationUnit::State::Parsing);

    libclang::Index& cindex = m_index.libclangIndex();

    auto start = std::chrono::high_resolution_clock::now();

    auto clangtu = std::make_unique<libclang::TranslationUnit>(cindex.parseTranslationUnit(m_translation_unit.filePath().toStdString(),
      m_translation_unit.compileOptions().includedirs, CXTranslationUnit_DetailedPreprocessingRecord));

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Parsed " << m_translation_unit.filePath().toStdString() << " in " << duration.count() << "ms" << std::endl;

    m_translation_unit.setClangTranslationUnit(std::move(clangtu));
  }
};

class ReparseTranslationUnit : public QRunnable
{
private:
  ClangIndex& m_index;
  TranslationUnit& m_translation_unit;

public:

  explicit ReparseTranslationUnit(ClangIndex& index, TranslationUnit& tu) :
    m_index(index),
    m_translation_unit(tu)
  {
    setAutoDelete(true);
  }

  void run() override
  {
    m_translation_unit.setState(TranslationUnit::State::Parsing);

    libclang::TranslationUnit& tu = *m_translation_unit.clangTranslationUnit();
    tu.reparseTranslationUnit();

    m_translation_unit.setState(TranslationUnit::State::Loaded);
  }
};

TranslationUnitLoaderFactory::~TranslationUnitLoaderFactory()
{

}

QRunnable* TranslationUnitLoaderFactory::createLoader(ClangIndex& index, TranslationUnit& t)
{
  if (t.state() == TranslationUnit::State::AwaitingParsing)
    return new ParseTranslationUnit(index, t);
  else
    return new ReparseTranslationUnit(index, t);
}

ClangIndex::ClangIndex(LibClang& lib, QObject* parent) : QObject(parent),
  m_library(lib),
  m_loader_factory(std::make_unique<TranslationUnitLoaderFactory>()),
  m_check_parsing(this, "checkParsing"),
  m_unload_translation_units(this, "unloadTranslationUnits")
{
  if (!m_library.libclangAvailable())
    throw std::runtime_error("ClangIndex: libclang is not available");

  m_index = std::make_unique<libclang::Index>(m_library.libclang()->createIndex());

  m_thread_pool = new QThreadPool(this);
}

ClangIndex::~ClangIndex()
{
 //$todo: cancel parsing of translation units
  // wait for the threadpool
  
#ifdef CLARK_DEBUG_DESTRUCTORS
  qDebug() << "~ClangIndex()";
#endif
}

libclang::Index& ClangIndex::libclangIndex() const
{
  return *m_index;
}

void ClangIndex::addTranslationUnits(const std::vector<TranslationUnit*>& list, const program::CompileOptions& options)
{
  auto opts = std::make_shared<program::CompileOptions>(options);

  for (auto tu : list)
  {
    tu->setCompileOptions(opts);
    addToList(tu);
  }

  Q_EMIT translationUnitsAdded((int)list.size());
}

const std::vector<TranslationUnit*>& ClangIndex::translationUnits() const
{
  return m_translation_units;
}

void ClangIndex::setTranslationUnits(std::vector<TranslationUnit*> list)
{
  m_translation_units = std::move(list);

  for (TranslationUnit* tu : m_translation_units)
    manage(tu);
}

void ClangIndex::load(TranslationUnit* tu)
{
  TranslationUnit::State s = tu->state();

  if (s == TranslationUnit::State::Loaded || s == TranslationUnit::State::Parsing)
    return;

  std::lock_guard<std::mutex> lock{ m_mutex };

  auto it = std::find(m_translation_unit_parsing_queue.begin(), m_translation_unit_parsing_queue.end(), tu);

  if (it != m_translation_unit_parsing_queue.end())
    parse(it);
  else
    parse(tu);
}

TranslationUnitLoaderFactory& ClangIndex::loaderFactory() const
{
  return *m_loader_factory;
}

void ClangIndex::setLoaderFactory(std::unique_ptr<TranslationUnitLoaderFactory> factory)
{
  if (!factory)
    return;

  m_loader_factory = std::move(factory);
}

void ClangIndex::addToList(TranslationUnit* tu)
{
  m_translation_units.push_back(tu);
  manage(tu);
}

void ClangIndex::manage(TranslationUnit* tu)
{
  tu->setParent(this);
  tu->setClangIndex(this);
  connect(tu, &TranslationUnit::usedChanged, this, &ClangIndex::onTranslationUnitUsedChanged);

  if (tu->state() == TranslationUnit::AwaitingParsing)
  {
    // $todo: check if tu has data in db, otherwise parse
    // ...
    // in fact no,
    // if tu was loaded from database, it should already have the SavedIntoDatbase flag,
    // so check that instead right way
    scheduleParsing(tu);
  }
}

void ClangIndex::scheduleParsing(TranslationUnit* tu)
{
  std::lock_guard<std::mutex> lock{ m_mutex };
  m_translation_unit_parsing_queue.push_back(tu);
  m_check_parsing.scheduleCall();
}

void ClangIndex::checkParsing()
{
  m_check_parsing.clearCallFlag();

  std::lock_guard<std::mutex> lock{ m_mutex };

  if (m_translation_unit_parsing_queue.empty())
    return;

  int maxthread = std::max(m_thread_pool->maxThreadCount() - 1, 1);

  while (m_thread_pool->activeThreadCount() < maxthread && !m_translation_unit_parsing_queue.empty())
    parse(m_translation_unit_parsing_queue.begin());
}

void ClangIndex::parse(std::list<TranslationUnit*>::iterator it)
{
  TranslationUnit* tu = *it;
  m_translation_unit_parsing_queue.erase(it);
  parse(tu);
}

void ClangIndex::parse(TranslationUnit* tu)
{
  connect(tu, &TranslationUnit::loaded, this, &ClangIndex::onTranslationUnitParsed);
 
  QRunnable* task = loaderFactory().createLoader(*this, *tu);

  m_thread_pool->start(task);
}

void ClangIndex::checkUsed(TranslationUnit* tu)
{
  if (!tu->used())
  {
    m_translation_units_to_unload.push_back(tu);
    m_unload_translation_units.scheduleCall();
  }
}

void ClangIndex::unloadTranslationUnits()
{
  m_unload_translation_units.clearCallFlag();

  for (TranslationUnit* tu : m_translation_units_to_unload)
  {
    std::unique_lock lock{ tu->mutex() };

    bool unloaded = false;

    if (tu->data().use_count == 0 && tu->data().state == TranslationUnit::Loaded)
    {
      tu->data().clang_translation_unit->suspendTranslationUnit();

      tu->data().state = TranslationUnit::Suspended;

      unloaded = true;
    }

    lock.unlock();

    if (unloaded)
      Q_EMIT tu->stateChanged();
  }

  m_translation_units_to_unload.clear();
}

void ClangIndex::onTranslationUnitParsed()
{
  auto* tu = qobject_cast<TranslationUnit*>(sender());
 
  if (!tu)
    return;

  disconnect(tu, &TranslationUnit::loaded, this, &ClangIndex::onTranslationUnitParsed);

  m_check_parsing.scheduleCall();

  Q_EMIT translationUnitLoaded(tu);

  checkUsed(tu);
}

void ClangIndex::onTranslationUnitUsedChanged()
{
  auto* tu = qobject_cast<TranslationUnit*>(sender());

  if (!tu)
    return;

  checkUsed(tu);
}
