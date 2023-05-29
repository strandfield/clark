// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "codevieweractions.h"

#include "window.h"

#include "indexing/indexer.h"
#include "sema/tusymbol.h"

#include <QMenu>

CodeViewerClangActions::CodeViewerClangActions(Window& window, CodeViewer& viewer) : CodeViewerContextMenuHandler(viewer),
  m_window(window)
{

}

CodeViewerClangActions::~CodeViewerClangActions()
{

}

void CodeViewerClangActions::fill(QMenu* menu)
{
  SymbolObject* symbol = codeviewer().symbolUnderCursor();

  auto* clangsymbol = qobject_cast<TranslationUnitSymbolObject*>(symbol);

  if (!clangsymbol)
    return;

  if (!m_window.translationUnitIndexing() || !m_window.translationUnitIndexing()->isReady())
    return;

  const clark::IndexingResult& idx = m_window.translationUnitIndexing()->indexingResult();
  const clark::Entity* entity = clark::find_entity(idx, clangsymbol->cursor().getUSR());

  if (entity)
  {
    menu->addAction("Find references", [this, entity]() {
      m_window.createFindReferencesWidget(entity);
      });
  }
}
