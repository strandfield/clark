// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "findreferenceswidget.h"

#include <indexing/indexer.h>

#include <QTreeWidget>

#include <QVBoxLayout>

#include <QtConcurrent>

#include <algorithm>
#include <iterator>

static QList<clark::EntityReference> find_references(const clark::IndexingResult* index, const clark::Entity* entity)
{
  QList<clark::EntityReference> result;

  std::copy_if(index->references.begin(), index->references.end(), std::back_inserter(result), [entity](const clark::EntityReference& eref) {
    return eref.symbol == entity;
    });

  std::sort(result.begin(), result.end(), [](const clark::EntityReference& lhs, const clark::EntityReference& rhs) {
    return std::forward_as_tuple(lhs.file->path, lhs.line) < std::forward_as_tuple(rhs.file->path, rhs.line);
    });

  return result;
}


FindReferencesWidget::FindReferencesWidget(QWidget* parent) : FindReferencesWidget(nullptr, nullptr, parent)
{

}

FindReferencesWidget::FindReferencesWidget(TranslationUnitIndexing* idx, const clark::Entity* e, QWidget* parent) : QWidget(parent)
{
  m_tree_widget = new QTreeWidget;

  {
    m_tree_widget->setHeaderHidden(false);
    m_tree_widget->setHeaderLabels(QStringList() << "Code" << "Line" << "Read/Write");
    m_tree_widget->setColumnCount(3);
  }

  auto* layout = new QVBoxLayout;
  {
    layout->addWidget(m_tree_widget);
  }
  setLayout(layout);

  {
    connect(m_tree_widget, &QTreeWidget::itemDoubleClicked, this, &FindReferencesWidget::onTreeItemDoubleClicked);
  }

  setIndexing(idx);
  setEntity(e);
}

FindReferencesWidget::~FindReferencesWidget()
{
  if (m_references_future)
  {
    m_references_future->waitForFinished();
  }
}

TranslationUnitIndexing* FindReferencesWidget::indexing() const
{
  return m_indexing;
}

void FindReferencesWidget::setIndexing(TranslationUnitIndexing* idx)
{
  if (m_indexing == idx)
    return;

  m_indexing = idx;

  if (m_indexing)
  {
    connect(m_indexing, &QObject::destroyed, this, &FindReferencesWidget::clear);
  }

  computeReferences();
}

const clark::Entity* FindReferencesWidget::entity() const
{
  return m_entity;
}

void FindReferencesWidget::setEntity(const clark::Entity* e)
{
  if (m_entity == e)
    return;

  m_entity = e;

  computeReferences();
}

void FindReferencesWidget::clear()
{
  setIndexing(nullptr);
}

void FindReferencesWidget::computeReferences()
{
  if (!m_indexing)
  {
    fillTree({});
    return;
  }

  QFuture<QList<clark::EntityReference>> f = QtConcurrent::run(find_references, &indexing()->indexingResult(), entity());
  auto watcher = new QFutureWatcher<QList<clark::EntityReference>>(this);
  connect(watcher, &QFutureWatcher<QList<clark::EntityReference>>::finished, this, &FindReferencesWidget::onReferencesComputationFinished);
  watcher->setFuture(f);
  m_references_future = watcher;
}

void FindReferencesWidget::onReferencesComputationFinished()
{
  auto* watcher = qobject_cast<QFutureWatcherBase*>(sender());

  if (!watcher)
    return;

  watcher->deleteLater();

  if (watcher != m_references_future)
    return;

  QList<clark::EntityReference> references = m_references_future->result();

  m_references_future = nullptr;

  fillTree(references);
}

void FindReferencesWidget::onTreeItemDoubleClicked(QTreeWidgetItem* item)
{
  if (item->columnCount() == 3)
  {
    QString document = item->parent()->text(0);
    bool ok = false;
    int line = item->text(1).toInt(&ok);
    
    if (ok)
    {
      Q_EMIT referenceClicked(document, line);
    }
  }
}

void FindReferencesWidget::fillTree(const QList<clark::EntityReference>& references)
{
  m_tree_widget->clear();

  auto find_next_file = [&references](const QList<clark::EntityReference>::const_iterator it) {
    return std::find_if(it, references.end(), [&references, &it](const clark::EntityReference& eref) {
      return eref.file != it->file;
      });
  };

 auto it = references.begin();
 
 while (it != references.end())
 {
   QString filecontent = clark::get_file_contents(*indexing()->translationUnit().clangTranslationUnit(), *(it->file));
   QStringList lines = filecontent.split('\n');

   auto end = find_next_file(it);

   auto* file_item = new QTreeWidgetItem;
   file_item->setText(0, QString::fromStdString(it->file->path));

   std::for_each(it, end, [this, file_item, &lines](const clark::EntityReference& eref) {
     QTreeWidgetItem* item = createItem(eref, lines.at(eref.line - 1));
     file_item->addChild(item);
     });

   m_tree_widget->addTopLevelItem(file_item);

   it = end;
 }
}

QTreeWidgetItem* FindReferencesWidget::createItem(const clark::EntityReference& eref, const QString& linetext) const
{
  QString readwrite;

  if (eref.flags & clark::EntityReference::Write)
  {
    readwrite = "Write";
  }
  else if (eref.flags & clark::EntityReference::Read)
  {
    readwrite = "Read";
  }
  else if (eref.flags & clark::EntityReference::Call)
  {
    readwrite = "Call";
  }
  else if (eref.flags & clark::EntityReference::AddressOf)
  {
    readwrite = "AddressOf";
  }
  else if (eref.flags & clark::EntityReference::Definition)
  {
    readwrite = "Definition";
  }
  else if (eref.flags & clark::EntityReference::Declaration)
  {
    readwrite = "Declaration";
  }
  else
  {
    readwrite = "Other";
  }

  if (eref.flags & clark::EntityReference::Implicit)
  {
    readwrite += " (Implicit)";
    readwrite = readwrite.simplified();
  }

  auto* result = new QTreeWidgetItem;

  result->setText(0, linetext);
  result->setText(1, QString::number(eref.line));
  result->setText(2, readwrite);

  return result;
}
