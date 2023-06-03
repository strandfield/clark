// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QSettings;

class LibClang;

class LibClangPreferences : public QWidget
{
  Q_OBJECT
public:
  LibClangPreferences(QSettings& settings, LibClang& libclang, QWidget* parent = nullptr);

  static QString settingsPathKey();

protected Q_SLOTS:
  void updateStatus();

protected:
  void launchOpenLibClangDialog();
  void changeLibClangPath(const QString& path);

private:
  QSettings& m_settings;
  LibClang& m_libclang;
  QLineEdit* m_libclang_path_lineedit = nullptr;
  QPushButton* m_open_libclang_button = nullptr;
  QLabel* m_libclang_status_display = nullptr;
};
