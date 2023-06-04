// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "settingsdialog.h"

#include "program/libclang.h"

#include "settings/libclangpreferences.h"

#include "application.h"
#include "settings.h"

#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QSettings>

SettingsDialog::SettingsDialog(Application& app, QWidget* parent) : QDialog(parent)
{
  setWindowTitle("Settings");

  m_pages_widget = new QStackedWidget;
  m_pages_widget->addWidget(new LibClangPreferences(app.settings(), app.get<LibClang>()));

  m_pagelist_widget = new QListWidget;
  m_pagelist_widget->addItem("libclang");
  m_pagelist_widget->setFixedWidth(m_pagelist_widget->sizeHintForColumn(0) + 24);

  m_close_button = new QPushButton("Close");

  {
    auto* layout = new QVBoxLayout;

    {
      auto* sublayout = new QHBoxLayout;
      sublayout->addWidget(m_pagelist_widget);
      sublayout->addWidget(m_pages_widget, 1);
      layout->addLayout(sublayout);
    }

    layout->addWidget(m_close_button, 0, Qt::AlignHCenter);

    setLayout(layout);
  }

  {
    connect(m_pagelist_widget, &QListWidget::currentItemChanged, this, [this]() {
      m_pages_widget->setCurrentIndex(m_pagelist_widget->currentIndex().row());
      });
    connect(m_close_button, &QPushButton::clicked, this, &QDialog::accept);
  }
}
