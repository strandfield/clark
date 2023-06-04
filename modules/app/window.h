// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_WINDOW_H
#define CLARK_WINDOW_H

#include "program/translationunit.h"

#include <QMainWindow>

class QAction;

namespace libclang
{
class Cursor;
} // namespace libclang

namespace clark
{
struct Entity;
} // namespace clark

class Application;
class CodeViewer;
class TranslationUnitIndexing;

class Window : public QMainWindow
{
  Q_OBJECT
public:
  explicit Window(Application& app, TranslationUnit* tu = nullptr);

  TranslationUnit* translationUnit() const;
  void setTranslationUnit(TranslationUnit* tu);
  const TranslationUnitHandle& translationUnitHandle() const;
  QString translationUnitName() const;

  TranslationUnitIndexing* translationUnitIndexing() const;

  void closeAllDocuments();

  void createFindReferencesWidget(const clark::Entity* e);

protected Q_SLOTS:
  void about();
  void newTranslationUnit();
  void refreshUi();

protected:
  void setupUi();

protected:
  void showEvent(QShowEvent* ev) override;
  void closeEvent(QCloseEvent* ev) override;

protected Q_SLOTS:
  void onTranslationUnitLoaded();
  void onTranslationUnitIndexingReady();

protected:
  QDockWidget* dock(QWidget* w, Qt::DockWidgetArea area);
  void onTabCloseRequested(int index);
  void onHandleReady();

  void closeTranslationUnit();
  bool openDocument(const QString& path);
  bool openFileOnDisk(const QString& path);
  void gotoDocument(const QString& path);
  void gotoDocumentLine(const QString& path, int l);
  void addCodeviewer(CodeViewer* viewer, bool connectSignals = true);
  CodeViewer* findCodeviewer(const QString& path) const;
  void onSymbolClicked();

  void createFileWidget();

  void createAstView();
  void onCursorDblClicked(const libclang::Cursor& c);

  void createEntityView();

  void createDerivedClassesWidget();

  void checkLibClangPath();

  void openSettingsDialog();

private:
  TranslationUnit* m_translation_unit = nullptr;
  TranslationUnitHandle m_handle;
  TranslationUnitIndexing* m_translation_unit_indexing = nullptr;

private:
  Application& m_app;
  /* File menu */
  QAction* m_new_tu_action = nullptr;
  QAction* m_close_action = nullptr;
  /* View menu */
  QAction* m_view_files_action = nullptr;
  QAction* m_astview_action = nullptr;
  QAction* m_view_symbols_action = nullptr;
  QAction* m_view_derivedclasses_action = nullptr;
  /* Settings menu */
  QAction* m_settings_action = nullptr;
  /* Central widget */
  QTabWidget* m_documents_tab_widget = nullptr;
};

#endif // CLARK_WINDOW_H
