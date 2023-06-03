// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <QDialog>

class QListWidget;
class QPushButton;
class QStackedWidget;

class Application;

class SettingsDialog : public QDialog
{
  Q_OBJECT
public:
  explicit SettingsDialog(Application& app, QWidget* parent = nullptr);

private:
  QListWidget* m_pagelist_widget;
  QStackedWidget* m_pages_widget;
  QPushButton* m_close_button;
};
