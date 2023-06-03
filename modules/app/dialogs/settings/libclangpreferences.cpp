// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "libclangpreferences.h"

#include "program/libclang.h"

#include "settings.h"

#include <QFileDialog>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <QHBoxLayout>
#include <QVBoxLayout>

LibClangPreferences::LibClangPreferences(QSettings& settings, LibClang& libclang, QWidget* parent) : QWidget(parent),
  m_settings(settings),
  m_libclang(libclang)
{
  m_libclang_path_lineedit = new QLineEdit;
  m_libclang_path_lineedit->setText(settings.value(settingsPathKey(), "libclang").toString());

  m_open_libclang_button = new QPushButton("Open");

  m_libclang_status_display = new QLabel;

  {
    auto* layout = new QVBoxLayout;

    {
      auto* sublayout = new QHBoxLayout;
      sublayout->addWidget(new QLabel("libclang: "));
      sublayout->addWidget(m_libclang_path_lineedit, 1);
      sublayout->addWidget(m_open_libclang_button);
      layout->addLayout(sublayout);
    }

    {
      auto* sublayout = new QHBoxLayout;
      sublayout->addWidget(new QLabel("Status: "));
      sublayout->addWidget(m_libclang_status_display);
      sublayout->addStretch();
      layout->addLayout(sublayout);
    }

    layout->addStretch();

    setLayout(layout);
  }

  {
    connect(m_open_libclang_button, &QPushButton::clicked, this, &LibClangPreferences::launchOpenLibClangDialog);
    connect(m_libclang_path_lineedit, &QLineEdit::editingFinished, this, [this]() {
      changeLibClangPath(m_libclang_path_lineedit->text());
      });
  }

  updateStatus();
}

QString LibClangPreferences::settingsPathKey()
{
  return Settings::libclangPathKey();
}

void LibClangPreferences::updateStatus()
{
  m_libclang_status_display->setText(m_libclang.libclangAvailable() ? "OK" : "KO");
  QPalette p = m_libclang_status_display->palette();
  p.setColor(QPalette::WindowText, m_libclang.libclangAvailable() ? Qt::green : Qt::red);
  m_libclang_status_display->setPalette(p);
}

void LibClangPreferences::launchOpenLibClangDialog()
{
#ifdef  Q_OS_WIN
  QString filters = "libclang.dll";
#else
  QString filters = "libclang.so";
#endif //  QT_OS_WINDOWS

  QString path = QFileDialog::getOpenFileName(this, "Open libclang", QString(), filters);

  if (path.isEmpty())
    return;

  changeLibClangPath(path);
}

void LibClangPreferences::changeLibClangPath(const QString& path)
{
  m_libclang_path_lineedit->setText(path);

  m_libclang.setLibrary(LibClang::tryLoad(path));

  updateStatus();

  m_settings.setValue(settingsPathKey(), path);
}
