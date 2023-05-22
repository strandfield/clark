// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "filewidget.h"

#include <indexing/indexer.h>

FileWidget::FileWidget(QWidget* parent) : FileWidget(nullptr, parent)
{

}

FileWidget::FileWidget(TranslationUnitIndexing* tindex, QWidget* parent) : QListWidget(parent)
{
  setIndexing(tindex);

  connect(this, &QListWidget::itemDoubleClicked, this, &FileWidget::onItemDoubleClicked);
}

TranslationUnitIndexing* FileWidget::indexing() const
{
  return m_indexing;
}

void FileWidget::setIndexing(TranslationUnitIndexing* idx)
{
  if (m_indexing == idx)
    return;

  if (m_indexing)
  {
    clear();
    disconnect(m_indexing, nullptr, this, nullptr);
  }

  m_indexing = idx;

  if (m_indexing)
  {
    connect(m_indexing, &QObject::destroyed, this, [this]() {
      setIndexing(nullptr);
      });

    if (!m_indexing->isReady())
    {
      connect(m_indexing, &TranslationUnitIndexing::ready, this, &FileWidget::fillItems);
    }
    else
    {
      fillItems();
    }
  }
}

void FileWidget::onItemDoubleClicked(QListWidgetItem* item)
{
  if (item)
  {
    Q_EMIT fileDoubleClicked(item->text());
  }
}

void FileWidget::fillItems()
{
  clear();

  if (!indexing() || !indexing()->isReady())
    return;

  const clark::IndexingResult& results = indexing()->indexingResult();

  for (const auto& p : results.files)
  {
    addItem(QString::fromStdString(p.second->path));
  }
}