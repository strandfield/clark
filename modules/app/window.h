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
  void gotoDocument(const QString& path);
  void gotoDocumentLine(const QString& path, int l);
  CodeViewer* findCodeviewer(const QString& path) const;
  void onSymbolClicked();

  void createAstView();
  void onCursorDblClicked(const libclang::Cursor& c);

private:
  TranslationUnit* m_translation_unit = nullptr;
  TranslationUnitHandle m_handle;
  TranslationUnitIndexing* m_translation_unit_indexing = nullptr;

private:
  QAction* m_close_action = nullptr;
  QTabWidget* m_documents_tab_widget = nullptr;
};

#endif // CLARK_WINDOW_H
