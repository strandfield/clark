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

class CodeViewer;
class TranslationUnitIndexing;

class Window : public QMainWindow
{
  Q_OBJECT
public:
  explicit Window(TranslationUnit* tu = nullptr);

  TranslationUnit* translationUnit() const;
  void setTranslationUnit(TranslationUnit* tu);
  const TranslationUnitHandle& translationUnitHandle() const;
  QString translationUnitName() const;

  TranslationUnitIndexing* translationUnitIndexing() const;

  void closeAllDocuments();

protected Q_SLOTS:
  void about();
  void newTranslationUnit();
  void refreshUi();

protected:
  void setupUi();

protected:
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

private:
  TranslationUnit* m_translation_unit = nullptr;
  TranslationUnitHandle m_handle;
  TranslationUnitIndexing* m_translation_unit_indexing = nullptr;

private:
  /* File menu */
  QAction* m_close_action = nullptr;
  /* View menu */
  QAction* m_view_files_action = nullptr;
  QAction* m_astview_action = nullptr;
  QAction* m_view_symbols_action = nullptr;
  QAction* m_view_derivedclasses_action = nullptr;
  /* Central widget */
  QTabWidget* m_documents_tab_widget = nullptr;
};

#endif // CLARK_WINDOW_H
