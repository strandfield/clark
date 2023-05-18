// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "translationunit.h"

#include "clangindex.h"

#include <libclang-utils/clang-translation-unit.h>

#include <QThread>

#include <QDebug>

static const std::shared_ptr<const program::CompileOptions> gEmptyCompileOptions = {};

//TranslationUnit::Data::Data()
//{
//
//}

TranslationUnit::Data::~Data()
{

}

TranslationUnit::TranslationUnit(const QString& filePath, QObject* parent) :
  QObject(parent),
  m_file_path(filePath)
{

}

TranslationUnit::~TranslationUnit()
{
#ifdef CLARK_DEBUG_DESTRUCTORS
  qDebug() << "~TranslationUnit()";
#endif

  Q_EMIT aboutToBeDestroyed();

  if (m_data.use_count)
  {
    qDebug() << "Warning: translation unit is used while being destroyed";
  }
}

/**
 * \brief returns the path of the main source file of the translation unit
 */
const QString& TranslationUnit::filePath() const
{
  return m_file_path;
}

const program::CompileOptions& TranslationUnit::compileOptions() const
{
  return *m_compile_options;
}

void TranslationUnit::setCompileOptions(const program::CompileOptions& opts)
{
  setCompileOptions(std::make_shared<program::CompileOptions>(opts));
}

void TranslationUnit::setCompileOptions(std::shared_ptr<const program::CompileOptions> opts)
{
  if (!opts)
    m_compile_options = gEmptyCompileOptions;
  else
    m_compile_options = std::move(opts);
}

TranslationUnit::State TranslationUnit::state() const
{
  std::lock_guard<std::mutex> lock{ m_mutex };
  return m_data.state;
}

void TranslationUnit::setState(State s)
{
  std::unique_lock lock{ m_mutex };

  if (m_data.state != s)
  {
    m_data.state = s;
    lock.unlock();
    Q_EMIT stateChanged();

    if (s == State::Loaded)
      Q_EMIT loaded();
  }
}

void TranslationUnit::notifyStateChange()
{
  Q_EMIT stateChanged();
}

bool TranslationUnit::isLoaded() const
{
  return state() == State::Loaded;
}

void TranslationUnit::load()
{
  Q_ASSERT(clangIndex());

  if (isLoaded())
    return;

  clangIndex()->load(this);
}

int TranslationUnit::flags() const
{
  return m_data.flags;
}

void TranslationUnit::setFlag(Flag f, bool on)
{
  std::lock_guard<std::mutex> lock{ m_mutex };
 
  if (on)
    m_data.flags |= static_cast<int>(f);
  else
    m_data.flags &= ~static_cast<int>(f);
}

void TranslationUnit::setClangTranslationUnit(std::unique_ptr<libclang::TranslationUnit> tu)
{
  {
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_data.clang_translation_unit = std::move(tu);
    m_data.state = State::Loaded;
  }

  loadedConditionVariable().notify_all();
  
  emit stateChanged();
  emit loaded();
}

libclang::TranslationUnit* TranslationUnit::clangTranslationUnit() const
{
  return m_data.clang_translation_unit.get();
}

bool TranslationUnit::used() const
{
  return useCount() > 0;
}

int TranslationUnit::useCount() const
{
  std::lock_guard<std::mutex> lock{ m_mutex };
  return m_data.use_count;
}

void TranslationUnit::decrementUseCount()
{
  bool used_changed = false;

  {
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_data.use_count--;
    used_changed = m_data.use_count == 0;
  }

  if (used_changed)
    emit usedChanged();
}

ClangIndex* TranslationUnit::clangIndex() const
{
  return m_index;
}

void TranslationUnit::setClangIndex(ClangIndex* index)
{
  m_index = index;
}

std::mutex& TranslationUnit::mutex() const
{
  return m_mutex;
}

std::condition_variable& TranslationUnit::loadedConditionVariable()
{
  return m_loaded_condition_variable;
}

TranslationUnit::Data& TranslationUnit::data()
{
  return m_data;
}

const TranslationUnit::Data& TranslationUnit::data() const
{
  return m_data;
}

static bool is_tu_loaded(TranslationUnit& tu)
{
  return tu.data().state == TranslationUnit::Loaded;
}

static void load_tu(TranslationUnit& tu, std::unique_lock<std::mutex>& lock)
{
  tu.clangIndex()->load(&tu);
  tu.loadedConditionVariable().wait(lock, [&tu]() { return is_tu_loaded(tu); });
}

TranslationUnitHandle::TranslationUnitHandle(TranslationUnit& tu) :
  m_translation_unit(&tu)
{
  load(tu);
}

TranslationUnitHandle::TranslationUnitHandle(const TranslationUnitHandle& other) :
  m_translation_unit(other.m_translation_unit)
{
  if (m_translation_unit)
  {
    std::unique_lock lock{ m_translation_unit->mutex() };
    m_translation_unit->data().use_count += 1;
  }
}

TranslationUnitHandle::TranslationUnitHandle(TranslationUnitHandle&& other) :
  m_translation_unit(other.m_translation_unit)
{
  other.m_translation_unit = nullptr;
}

TranslationUnitHandle::~TranslationUnitHandle()
{
  if (m_translation_unit)
    m_translation_unit->decrementUseCount();
}

bool TranslationUnitHandle::valid() const
{
  return m_translation_unit && m_translation_unit->state() == TranslationUnit::Loaded;
}

TranslationUnit* TranslationUnitHandle::translationUnit() const
{
  return m_translation_unit;
}

libclang::TranslationUnit& TranslationUnitHandle::clangTranslationunit() const
{
  return *m_translation_unit->clangTranslationUnit();
}

void TranslationUnitHandle::reset(TranslationUnit* tu)
{
  if (tu)
  {
    reset(*tu);
  }
  else
  {
    if (m_translation_unit)
      m_translation_unit->decrementUseCount();

    m_translation_unit = nullptr;
  }
}

void TranslationUnitHandle::reset(TranslationUnit& tu)
{
  if (&tu == m_translation_unit)
    return;

  if (m_translation_unit)
    m_translation_unit->decrementUseCount();

  m_translation_unit = &tu;
  load(*m_translation_unit);
}

static TranslationUnitHandle::LoadProc& thread_loadproc()
{
  thread_local TranslationUnitHandle::LoadProc lp = &load_tu;
  return lp;
}

void TranslationUnitHandle::setLoadProcForCurrentThread(LoadProc proc)
{
  thread_loadproc() = proc;
}

TranslationUnitHandle::LoadProc TranslationUnitHandle::getLoadProc()
{
  return thread_loadproc();
}

TranslationUnitHandle& TranslationUnitHandle::operator=(const TranslationUnitHandle& other)
{
  if (this == &other)
    return *this;

  if (m_translation_unit)
    m_translation_unit->decrementUseCount();

  m_translation_unit = nullptr;

  m_translation_unit = other.m_translation_unit;

  if (m_translation_unit)
    load(*m_translation_unit);

  return *this;
}

TranslationUnitHandle& TranslationUnitHandle::operator=(TranslationUnitHandle&& other)
{
  if (&other == this)
    return *this;

  if (m_translation_unit)
    m_translation_unit->decrementUseCount();

  m_translation_unit = other.m_translation_unit;
  other.m_translation_unit = nullptr;

  return *this;
}

void TranslationUnitHandle::load(TranslationUnit& tu)
{
  std::unique_lock lock{ tu.mutex() };

  if (!is_tu_loaded(tu))
  {
    LoadProc proc = getLoadProc();
    proc(tu, lock);
  }

  tu.data().use_count += 1;

  bool now_used = tu.data().use_count == 1;

  lock.unlock();

  if (now_used)
    Q_EMIT tu.usedChanged();
}