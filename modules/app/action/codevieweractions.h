// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "codeviewer/codeviewer.h"

class Window;

class CodeViewerClangActions : public CodeViewerContextMenuHandler
{
  Q_OBJECT

public:
  explicit CodeViewerClangActions(Window& window, CodeViewer& viewer);
  ~CodeViewerClangActions();

protected:
  void fill(QMenu* menu) override;

private:
  Window& m_window;
};