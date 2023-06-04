// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <QListWidget>

class TranslationUnitIndexing;

class FileWidget : public QListWidget
{
  Q_OBJECT
public:
  explicit FileWidget(QWidget* parent = nullptr);
  FileWidget(TranslationUnitIndexing* tindex, QWidget* parent = nullptr);

  TranslationUnitIndexing* indexing() const;
  void setIndexing(TranslationUnitIndexing* idx);

Q_SIGNALS:
  void fileDoubleClicked(const QString& path);

protected Q_SLOTS:
  void onItemDoubleClicked(QListWidgetItem* item);

protected:
  void fillItems();

private:
  TranslationUnitIndexing* m_indexing = nullptr;
};
