// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "window.h"

#include "action/codevieweractions.h"

#include "dialogs/aboutdialog.h"
#include "dialogs/openslndialog.h"
#include "dialogs/settingsdialog.h"

#include "view/astview.h"
#include "view/entityview.h"

#include "widget/clangfileviewer.h"
#include "widget/derivedclasseswidget.h"
#include "widget/filewidget.h"
#include "widget/findreferenceswidget.h"

#include "application.h"
#include "settings.h"

#include <sema/clangsemainfoprovider.h>

#include <indexing/indexer.h>

#include <codeviewer/codeviewer.h>
#include <codeviewer/syntaxhighlighter.h>

#include <program/clangindex.h>
#include <program/libclang.h>

#include <utils/io.h>

#include <libclang-utils/clang-translation-unit.h>

#include <QAction>
#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>

#include <QFileDialog>
#include <QMessageBox>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QTimer>

#include <QDebug>

Window::Window(Application& app, TranslationUnit* tu) :
  m_app(app)
{
  setWindowTitle("Clark");
  setAttribute(Qt::WA_DeleteOnClose);

  setupUi();

  if (tu)
    setTranslationUnit(tu);

  connect(m_app.find<LibClang>(), &LibClang::libclangAvailableChanged, this, &Window::refreshUi);
}

void Window::setupUi()
{
  {
    QMenu* menu = menuBar()->addMenu("&File");
    m_new_tu_action = menu->addAction("New Translation Unit...", this, &Window::newTranslationUnit);

    m_close_action = menu->addAction("&Close", this, &Window::closeTranslationUnit);

    menu->addSeparator();
    menu->addAction("&Exit", &m_app, &QApplication::quit)->setShortcut(QKeySequence("Alt+F4"));
  }

  {
    QMenu* menu = menuBar()->addMenu("&View");

    m_view_files_action = menu->addAction("&File", this, &Window::createFileWidget);
    m_astview_action = menu->addAction("AST", this, &Window::createAstView);
    m_view_symbols_action = menu->addAction("Symbols", this, &Window::createEntityView);
    m_view_derivedclasses_action = menu->addAction("Derived classes", this, &Window::createDerivedClassesWidget);
  }

  {
    m_settings_action = menuBar()->addAction("Settings", this, &Window::openSettingsDialog);
  }

  {
    QMenu* menu = menuBar()->addMenu("&Help");
    menu->addAction("About Qt", &m_app, &QApplication::aboutQt);
    menu->addAction("About", this, &Window::about);
  }

  m_documents_tab_widget = new QTabWidget;
  m_documents_tab_widget->setTabsClosable(true); // $todo: we may want something more granular
  connect(m_documents_tab_widget, &QTabWidget::tabCloseRequested, this, &Window::onTabCloseRequested);

  setCentralWidget(m_documents_tab_widget);

  statusBar()->showMessage("Hello World!", 500);

  refreshUi();

  QVariant geom = m_app.settings().value("window/geometry");

  if (geom.isValid())
    restoreGeometry(geom.toByteArray());
}

TranslationUnit* Window::translationUnit() const
{
  return m_translation_unit;
}

void Window::setTranslationUnit(TranslationUnit* tu)
{
  if (m_translation_unit != tu)
  {
    closeTranslationUnit();

    m_translation_unit = tu;

    if (m_translation_unit)
    {
      if (!m_translation_unit->clangIndex())
      {
        LibClang& lib = m_app.get<LibClang>();
        auto* index = new ClangIndex(lib, this);
        m_translation_unit->setClangIndex(index);
      }

      if (m_translation_unit->isLoaded())
      {
        m_handle.reset(*m_translation_unit);
      }
      else
      {
        connect(tu, &TranslationUnit::loaded, this, &Window::onTranslationUnitLoaded);
        tu->load();
        statusBar()->showMessage("Parsing translation unit...");
      }

      openDocument(tu->filePath());

      setWindowTitle("Clark - " + translationUnitName());
    }
  }
}

const TranslationUnitHandle& Window::translationUnitHandle() const
{
  return m_handle;
}

QString Window::translationUnitName() const
{
  return QFileInfo(translationUnit()->filePath()).fileName();
}

TranslationUnitIndexing* Window::translationUnitIndexing() const
{
  return m_translation_unit_indexing;
}

void Window::about()
{
  auto* dialog = new AboutDialog(this);
  dialog->setAttribute(Qt::WA_DeleteOnClose, true);
  dialog->open();
}

void Window::newTranslationUnit()
{
  QString path = QFileDialog::getOpenFileName(this, "Open Visual Studio Solution", QString(), QString("Visual Studio Solution (*.sln)"));

  if (path.isEmpty())
    return;

  vcxproj::Solution solution;

  try
  {
    solution = vcxproj::load_solution(path.toStdString());
  }
  catch (...)
  {
    QMessageBox::warning(this, "Visual Studio Solution", "Failed to open Visual Studio Solution.", QMessageBox::Ok);
    return;
  }

  OpenSlnDialog dialog{ solution, this };
  
  if (dialog.exec() != QDialog::Accepted)
    return;

  auto tu = dialog.result();

  if (tu)
  {
    tu->setParent(this);
    setTranslationUnit(tu.release());
  }
}

void Window::showEvent(QShowEvent* ev)
{
  QMainWindow::showEvent(ev);

  QTimer::singleShot(1000, this, &Window::checkLibClangPath);
}

void Window::closeEvent(QCloseEvent* ev)
{
  m_app.settings().setValue("window/geometry", saveGeometry());

  closeTranslationUnit();

  QMainWindow::closeEvent(ev);
}

void Window::refreshUi()
{
  bool has_tunit = translationUnit() != nullptr;
  bool has_idx = translationUnitIndexing() != nullptr;

  m_new_tu_action->setEnabled(m_app.get<LibClang>().libclangAvailable());
  m_close_action->setEnabled(has_tunit);
  m_view_files_action->setEnabled(has_idx);
  m_astview_action->setEnabled(has_tunit);
  m_view_symbols_action->setEnabled(has_idx);
  m_view_derivedclasses_action->setEnabled(has_idx);
}

void Window::onTranslationUnitLoaded()
{
  m_handle.reset(*translationUnit());
  disconnect(m_translation_unit, &TranslationUnit::loaded, this, &Window::onTranslationUnitLoaded);
  onHandleReady();

  statusBar()->showMessage("Done!", 500);

  m_translation_unit_indexing = new TranslationUnitIndexing(*m_translation_unit, this);

  connect(m_translation_unit_indexing, &TranslationUnitIndexing::started, this, [this]() {
    statusBar()->showMessage("Indexing...");
    });

  connect(m_translation_unit_indexing, &TranslationUnitIndexing::ready, this, &Window::onTranslationUnitIndexingReady);

  m_translation_unit_indexing->start();

  refreshUi();
}

void Window::onTranslationUnitIndexingReady()
{
  const clark::IndexingResult& idx = translationUnitIndexing()->indexingResult();
  int duration = std::chrono::duration_cast<std::chrono::milliseconds>(idx.indexing_time).count();
  statusBar()->showMessage(QString("Indexing completed! (%1ms)").arg(QString::number(duration)), 500);

  refreshUi();
}

QDockWidget* Window::dock(QWidget* w, Qt::DockWidgetArea area)
{
  auto* dock = new QDockWidget;
  dock->setWindowTitle(w->windowTitle());
  dock->setWidget(w);
  dock->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  addDockWidget(area, dock);
  return dock;
}

void Window::onTabCloseRequested(int index)
{
  m_documents_tab_widget->widget(index)->deleteLater();
  m_documents_tab_widget->removeTab(index);
}

void Window::closeAllDocuments()
{
  // Delete all widgets that may hold a handle to a translation unit
  while (m_documents_tab_widget->count() > 0)
  {
    m_documents_tab_widget->widget(0)->deleteLater();
    m_documents_tab_widget->removeTab(0);
  }
}

void Window::onHandleReady()
{
  for (int i(0); i < m_documents_tab_widget->count(); ++i)
  {
    auto* viewer = qobject_cast<CodeViewer*>(m_documents_tab_widget->widget(i));

    if (!viewer || qobject_cast<ClangFileViewer*>(viewer))
      continue;

    libclang::File f = translationUnitHandle().clangTranslationunit().getFile(viewer->documentPath().toStdString());

    if (!f.data)
    {
      qDebug() << "Translation unit is valid but could not find file: " << viewer->documentPath();
      continue;
    }

    ClangFileViewer::setup(viewer, translationUnitHandle(), f);
    connect(viewer, &CodeViewer::symbolUnderCursorClicked, this, &Window::onSymbolClicked);
    connect(viewer, &CodeViewer::includeDirectiveClicked, this, &Window::gotoDocument);
  }
}

void Window::closeTranslationUnit()
{
  if (!m_translation_unit)
    return;

  closeAllDocuments();
  
  if (m_translation_unit_indexing)
  {
    if (m_translation_unit_indexing->parent() == this)
      delete m_translation_unit_indexing;

    m_translation_unit_indexing = nullptr;
  }
  
  m_handle.reset();

  if (m_translation_unit->parent() == this)
    m_translation_unit->deleteLater();

  m_translation_unit = nullptr;

  refreshUi();
  setWindowTitle("Clark");
}

bool Window::openDocument(const QString& path)
{
  if (translationUnitHandle().valid())
  {
    libclang::File f = translationUnitHandle().clangTranslationunit().getFile(path.toStdString());

    if (!f.data)
    {
      qDebug() << "Translation unit is valid but could not find file: " << path;
      return openFileOnDisk(path);
    }

    auto* viewer = new ClangFileViewer(translationUnitHandle(), f);

    addCodeviewer(viewer);

    return true;
  }
  else
  {
    return openFileOnDisk(path);
  }
}

bool Window::openFileOnDisk(const QString& path)
{
  if (!clark::io::exists(path))
    return false;

  QString document_content = QString::fromUtf8(clark::io::read_from_disk(path));

  auto* viewer = new CodeViewer(path, document_content);

  constexpr bool connect_signals = false;
  addCodeviewer(viewer, connect_signals);

  return true;
}

void Window::gotoDocument(const QString& path)
{
  gotoDocumentLine(path, -1);
}

void Window::gotoDocumentLine(const QString& path, int l)
{
  CodeViewer* widget = findCodeviewer(path);

  if (widget)
  {
    m_documents_tab_widget->setCurrentWidget(widget);
  }
  else
  {
    if (openDocument(path))
    {
      widget = findCodeviewer(path);
      m_documents_tab_widget->setCurrentIndex(m_documents_tab_widget->count() - 1);
    }
  }

  if (widget && l != -1 && l > 0)
  {
    widget->goToLine(l);
  }
}

void Window::addCodeviewer(CodeViewer* viewer, bool connectSignals)
{
  viewer->addContextMenuHandler<CodeViewerClangActions>(*this);

  QString path = viewer->documentPath();
  int tabindex = m_documents_tab_widget->addTab(viewer, QFileInfo(path).fileName());
  m_documents_tab_widget->setTabToolTip(tabindex, path);

  if (connectSignals)
  {
    connect(viewer, &CodeViewer::symbolUnderCursorClicked, this, &Window::onSymbolClicked);
    connect(viewer, &CodeViewer::includeDirectiveClicked, this, &Window::gotoDocument);
  }
}

CodeViewer* Window::findCodeviewer(const QString& path) const
{
  if (path.contains('\\'))
  {
    return findCodeviewer(QString(path).replace('\\', '/'));
  }

  for (int i(0); i < m_documents_tab_widget->count(); ++i)
  {
    auto* cv = qobject_cast<CodeViewer*>(m_documents_tab_widget->widget(i));

    if (cv && cv->documentPath() == path)
    {
      return cv;
    }
  }

  return nullptr;
}

void Window::onSymbolClicked()
{
  auto* viewer = qobject_cast<CodeViewer*>(sender());

  if (!viewer)
    return;

  SymbolObject* symbol = viewer->symbolUnderCursor();

  if (!symbol)
    return;

  if (m_translation_unit_indexing && m_translation_unit_indexing->isReady())
  {
    const clark::IndexingResult& idx = m_translation_unit_indexing->indexingResult();
    const clark::Entity* entity = clark::find_entity(idx, symbol->usr().toStdString());

    if (!entity)
      return;

    const clark::EntityReference* def = clark::find_definition(idx, *entity);

    if (def)
    {
      gotoDocumentLine(QString::fromStdString(def->file->path), def->line);
    }
  }
}

void Window::createFileWidget()
{
  auto* v = new FileWidget(translationUnitIndexing());
  v->setWindowTitle("Files");
  connect(v, &FileWidget::fileDoubleClicked, this, &Window::gotoDocument);
  QDockWidget* widget = dock(v, Qt::DockWidgetArea::RightDockWidgetArea);

  connect(translationUnitIndexing(), &QObject::destroyed, this, [this, widget]() {
    removeDockWidget(widget);
    delete widget;
    });
}

void Window::createAstView()
{
  auto* v = new AstView(*translationUnit());
  v->setWindowTitle("AST");
  connect(v, &AstView::clangCursorDoubleClicked, this, &Window::onCursorDblClicked);
  QDockWidget* widget = dock(v, Qt::DockWidgetArea::RightDockWidgetArea);

  connect(translationUnit(), &TranslationUnit::aboutToBeDestroyed, this, [this, widget]() {
    removeDockWidget(widget);
    delete widget;
    });
}

void Window::onCursorDblClicked(const libclang::Cursor& c)
{
  libclang::SpellingLocation loc = c.getLocation().getSpellingLocation();
  std::string path = std::filesystem::path(loc.file.getFileName()).generic_u8string();
  gotoDocumentLine(QString::fromStdString(path), loc.line);
}

void Window::createEntityView()
{
  auto* v = new EntityView(translationUnitIndexing());
  v->setWindowTitle("Symbols");
  //connect(v, &EntityView::entityDoubleClicked, this, &Window::onEntityDblClicked);
  QDockWidget* widget = dock(v, Qt::DockWidgetArea::RightDockWidgetArea);

  connect(translationUnit(), &TranslationUnit::aboutToBeDestroyed, this, [this, widget]() {
    removeDockWidget(widget);
    delete widget;
    });
}

void Window::createDerivedClassesWidget()
{
  auto* v = new DerivedClassesWidget(translationUnitIndexing());
  v->setWindowTitle("Derived classes");
  QDockWidget* widget = dock(v, Qt::DockWidgetArea::RightDockWidgetArea);

  connect(translationUnit(), &TranslationUnit::aboutToBeDestroyed, this, [this, widget]() {
    removeDockWidget(widget);
    delete widget;
    });
}

void Window::createFindReferencesWidget(const clark::Entity* e)
{
  auto* v = new FindReferencesWidget(translationUnitIndexing(), e);

  connect(v, &FindReferencesWidget::referenceClicked, this, &Window::gotoDocumentLine);

  v->setWindowTitle("Find References '" + QString::fromStdString(e->display_name) + "'");
  QDockWidget* widget = dock(v, Qt::DockWidgetArea::BottomDockWidgetArea);

  connect(translationUnit(), &TranslationUnit::aboutToBeDestroyed, this, [this, widget]() {
    removeDockWidget(widget);
    delete widget;
    });
}

void Window::checkLibClangPath()
{
  if (!m_app.get<LibClang>().libclangAvailable())
  {
    const char* mssg =
      "libclang could not be loaded.\n"
      "Please specify the path to libclang in the Settings dialog.\n"
      "Do you wish to open the settings dialog now ?";

    int btn = QMessageBox::warning(this, "libclang missing", mssg, QMessageBox::Yes, QMessageBox::No);

    if (btn == QMessageBox::Yes)
    {
      openSettingsDialog();
    }
  }
}

void Window::openSettingsDialog()
{
  SettingsDialog dialog{ m_app };
  dialog.exec();
}
