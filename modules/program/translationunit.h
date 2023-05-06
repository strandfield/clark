// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_TRANSLATIONUNIT_H
#define CLARK_TRANSLATIONUNIT_H

#include <QObject>

#include <chrono>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

namespace libclang
{
class TranslationUnit;
} // namespace libclang

namespace program
{

/**
 * \brief stores various information about the compiler flags required to compile a translation unit
 */
struct CompileOptions
{
  /**
   * \brief lists the include directories used when compiling the translation unit
   */
  std::set<std::string> includedirs;

  /**
   * \brief lists the preprocessor defines used when compiling
   * 
   * The key is the name of the define/macro.
   * The value is the define's value and can be an empty string.
   */
  std::map<std::string, std::string> defines;
};

} // namespace program

class ClangIndex;

/**
 * \brief represents a C++ translation unit
 */
class TranslationUnit : public QObject
{
  Q_OBJECT
public:

  explicit TranslationUnit(const QString& filePath, QObject* parent = nullptr);
  ~TranslationUnit();

  const QString& filePath() const;

  const program::CompileOptions& compileOptions() const;
  void setCompileOptions(const program::CompileOptions& opts);
  void setCompileOptions(std::shared_ptr<const program::CompileOptions> opts);

  enum State
  {
    AwaitingParsing, // $todo: replace by flag "NeverParsed"
    Parsing,
    Loaded,
    Suspended,
    // $todo: maybe just NotLoaded, Loading, Loaded
  };

  State state() const;
  void setState(State s);
  void notifyStateChange();

  bool isLoaded() const;
  void load();

  enum Flag
  {
    // $todo: maybe Suspended, NerverParsed, ScheduleForParsing
  };

  int flags() const;
  void setFlag(Flag f, bool on = true);

  void setClangTranslationUnit(std::unique_ptr<libclang::TranslationUnit> tu);
  libclang::TranslationUnit* clangTranslationUnit() const;

  bool used() const;
  int useCount() const;
  void decrementUseCount();

  ClangIndex* clangIndex() const;
  void setClangIndex(ClangIndex* index);

  class Data
  {
  public:
    State state = AwaitingParsing;
    int flags = 0;
    int use_count = 0;
    std::unique_ptr<libclang::TranslationUnit> clang_translation_unit;

  public:
    //Data();
    ~Data();
    /*
    Data(const Data&) = delete;
    Data(Data&&) = delete;
    Data& operator=(Data&&) = delete;
    Data& operator=(const Data&) = delete;
    */
  };

  std::mutex& mutex() const;
  std::condition_variable& loadedConditionVariable();
  Data& data();
  const Data& data() const;

Q_SIGNALS:
  void aboutToBeDestroyed();
  void stateChanged();
  void loaded();
  void usedChanged();

private:
  QString m_file_path;
  std::shared_ptr<const program::CompileOptions> m_compile_options;
  ClangIndex* m_index = nullptr;
  mutable std::mutex m_mutex;
  std::condition_variable m_loaded_condition_variable;
  Data m_data;
};

/**
 * \brief a handle to a loaded translation unit
 * 
 * This class can be used to hold a reference to a TranslationUnit 
 * and ensure that TranslationUnit::clangTranslationUnit() will return 
 * a valid pointer.
 */
class TranslationUnitHandle
{
public:
  TranslationUnitHandle() = default;
  TranslationUnitHandle(const TranslationUnitHandle& other);
  TranslationUnitHandle(TranslationUnitHandle&& other);
  ~TranslationUnitHandle();

  explicit TranslationUnitHandle(TranslationUnit& tu);

  typedef void(*LoadProc)(TranslationUnit&, std::unique_lock<std::mutex>&);

  bool valid() const;

  TranslationUnit* translationUnit() const;
  libclang::TranslationUnit& clangTranslationunit() const;

  void reset(TranslationUnit* tu = nullptr);
  void reset(TranslationUnit& tu);

  static void setLoadProcForCurrentThread(LoadProc proc);
  static LoadProc getLoadProc();

  TranslationUnitHandle& operator=(const TranslationUnitHandle& other);
  TranslationUnitHandle& operator=(TranslationUnitHandle&& other);

private:
  void load(TranslationUnit& tu);

private:
  TranslationUnit* m_translation_unit = nullptr;
};

#endif // CLARK_TRANSLATIONUNIT_H
