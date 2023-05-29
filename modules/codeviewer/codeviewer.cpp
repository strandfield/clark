// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "codeviewer.h"

#include "includes.h"
#include "syntaxhighlighter.h"
#include "symbolinfoprovider.h"

#include <QFile>
#include <QMenu>

#include <QDebug>

CodeViewer::CodeViewer(const QString& documentPath, const QString& documentContent, QWidget* parent) : QPlainTextEdit(parent)
{
  document()->setDefaultFont(courierFont());

  setReadOnly(true);
  setMouseTracking(true);

  setPlainText(documentContent);
  document()->setMetaInformation(QTextDocument::MetaInformation::DocumentUrl, QString(documentPath).replace('\\', '/'));

  m_syntax_highlighter = new CpptokSyntaxHighlighter(document());
}

/**
 * \brief returns the path of the document
 * 
 * This is the "generic" path (i.e., with forward slashes).
 */
QString CodeViewer::documentPath() const
{
  return document()->metaInformation(QTextDocument::MetaInformation::DocumentUrl);
}

CppSyntaxHighlighter* CodeViewer::syntaxHighlighter() const
{
  return m_syntax_highlighter;
}

void CodeViewer::setSyntaxHighlighter(CppSyntaxHighlighter* highlighter)
{
  if (m_syntax_highlighter)
  {
    delete m_syntax_highlighter;
  }

  m_syntax_highlighter = highlighter;

  if (m_syntax_highlighter)
    m_syntax_highlighter->rehighlight();
}

QFont CodeViewer::courierFont()
{
  static QFont cache = []() -> QFont {
    QFont font{ "Courier" };
    font.setStyleHint(QFont::StyleHint::TypeWriter);
    return font;
  }();
  return cache;
}

SymbolInfoProvider* CodeViewer::symbolInfoProvider() const
{
  return m_info_provider;
}

void CodeViewer::setSymbolInfoProvider(SymbolInfoProvider* provider)
{
  clearTokenUnderCursor();

  if (m_info_provider)
  {
    m_info_provider->deleteLater();
    m_info_provider = nullptr;
  }

  m_info_provider = provider;

  if (m_info_provider)
  {
    m_info_provider->setParent(this);
  
    fetchIncludes();
  }
}

static bool contains(int startCol, int span, int col)
{
  return col >= startCol && col < startCol + span;
}

SymbolObject* CodeViewer::symbolUnderCursor() const
{
  return m_token_under_cursor.symbol;
}

void CodeViewer::goToLine(int l)
{
  moveCursor(QTextCursor::End);

  QTextBlock block = document()->findBlockByNumber(l - 1);
  QTextCursor cursor{ block };
  setTextCursor(cursor);
}

void CodeViewer::mousePressEvent(QMouseEvent* ev)
{
  if (symbolUnderCursor() && ev->button() == Qt::LeftButton)
    Q_EMIT symbolUnderCursorClicked();
  else if (!m_token_under_cursor.included_file.isNull() && ev->button() == Qt::LeftButton)
    Q_EMIT includeDirectiveClicked(m_token_under_cursor.included_file);
  else
    QPlainTextEdit::mousePressEvent(ev);
}

void CodeViewer::mouseMoveEvent(QMouseEvent* ev)
{
  QTextCursor c = cursorForPosition(ev->pos());
  updateTokenUnderCursor(c);

  QPlainTextEdit::mouseMoveEvent(ev);
}

void CodeViewer::contextMenuEvent(QContextMenuEvent* ev)
{
  QMenu* menu = createStandardContextMenu(ev->pos());

  Q_EMIT contextMenuRequested(menu);

  menu->exec(ev->globalPos());

  delete menu;
}

void CodeViewer::updateTokenUnderCursor(const QTextCursor& cursor)
{
  if (!symbolInfoProvider())
    return;

  QTextBlock block = cursor.block();
  QString text = block.text();
  int line = cursor.block().blockNumber() + 1;
  int col = cursor.positionInBlock() + 1;

  // Check if it's the same token as before
  if (line == m_token_under_cursor.line && contains(m_token_under_cursor.col, m_token_under_cursor.span, col))
    return;

  QStringLineTokenizer tokenizer;
  tokenizer.tokenize(cursor.block().text());

  auto tokit = tokenizer.tokenAt(col);

  if (tokit == tokenizer.lexer.output.end())
  {
    return setTokenUnderCursor(line, col, 1, nullptr);
  }

  col = tokenizer.col(*tokit);
  int span = tokenizer.length(*tokit);

  if (tokit->type() == cpptok::TokenType::Include)
  {
    QString inc = m_includes ? m_includes->getInclude(line) : QString();
    if(!inc.isNull())
      setTokenUnderCursor(line, col, span, inc);
    else
      setTokenUnderCursor(line, col, span, nullptr);
  }
  else if (tokit->isIdentifier())
  {
    TokenInfo tokinfo;
    tokinfo.column = col;
    tokinfo.line = line;
    tokinfo.document = document();
    tokinfo.token = *tokit;

    SymbolObject* sym = symbolInfoProvider()->getSymbol(tokinfo);
    setTokenUnderCursor(line, col, span, sym);
  }
}

void CodeViewer::setTokenUnderCursor(int line, int col, int span, SymbolObject* sym)
{
  m_token_under_cursor.line = line;
  m_token_under_cursor.col = col;
  m_token_under_cursor.span = span;

  setIncludedFileUnderCursor(QString());
  setSymbolUnderCursor(sym);
}

void CodeViewer::setTokenUnderCursor(int line, int col, int span, QString includedFile)
{
  m_token_under_cursor.line = line;
  m_token_under_cursor.col = col;
  m_token_under_cursor.span = span;

  setSymbolUnderCursor(nullptr);
  setIncludedFileUnderCursor(includedFile);
}

void CodeViewer::clearTokenUnderCursor()
{
  setTokenUnderCursor(-1, -1, 0, nullptr);
}

void CodeViewer::setSymbolUnderCursor(SymbolObject* sym)
{
  if (m_token_under_cursor.symbol != sym)
  {
    if (m_token_under_cursor.symbol && sym && m_token_under_cursor.symbol->isSame(*sym))
    {
      if (sym != m_token_under_cursor.symbol)
        sym->deleteLater();
      return;
    }

    if (m_token_under_cursor.symbol)
    {
      disconnect(m_token_under_cursor.symbol, nullptr, this, nullptr);
      m_token_under_cursor.symbol->deleteLater();
    }

    m_token_under_cursor.symbol = sym;

    if (m_token_under_cursor.symbol)
      m_token_under_cursor.symbol->setParent(this);

    onSymbolUnderCursorChanged();
    Q_EMIT symbolUnderCursorChanged();
  }
}

void CodeViewer::onSymbolUnderCursorChanged()
{
  if (m_token_under_cursor.references)
  {
    m_token_under_cursor.references->deleteLater();
    m_token_under_cursor.references = nullptr;
    refreshExtraSelections();
  }

  if (symbolUnderCursor())
  {
    m_token_under_cursor.references = symbolInfoProvider()->getReferencesInDocument(symbolUnderCursor(), documentPath());

    if (m_token_under_cursor.references)
    {
      m_token_under_cursor.references->setParent(this);

      if (!m_token_under_cursor.references->isComplete())
      {
        connect(m_token_under_cursor.references, &SymbolReferencesInDocument::changed, this, &CodeViewer::onSymbolUnderCursorInfoAvailable);
      }
      else
      {
        onSymbolUnderCursorInfoAvailable();
      }
    }
  }

  // $todo: only use PointingHandCursor if an action is possible
  // maybe it's always the case, though
  viewport()->setCursor(symbolUnderCursor() ? Qt::PointingHandCursor : Qt::IBeamCursor);
}

void CodeViewer::onSymbolUnderCursorInfoAvailable()
{
  refreshExtraSelections();
}

void CodeViewer::setIncludedFileUnderCursor(const QString& filePath)
{
  if (m_token_under_cursor.included_file != filePath)
  {
    m_token_under_cursor.included_file = filePath;
    onIncludeUnderCursorChanged();
  }
}

void CodeViewer::onIncludeUnderCursorChanged()
{
  viewport()->setCursor(!m_token_under_cursor.included_file.isEmpty() ? Qt::PointingHandCursor : Qt::IBeamCursor);
}

void CodeViewer::clearIncludes()
{
  if (m_includes)
  {
    if (m_includes->parent() == this)
    {
      m_includes->deleteLater();
    }

    m_includes = nullptr;
  }
}

void CodeViewer::fetchIncludes()
{
  if (!symbolInfoProvider())
    return;

  m_includes = symbolInfoProvider()->getIncludesInFile(documentPath());

  if (m_includes)
  {
    m_includes->setParent(this);
  }
}

void CodeViewer::refreshExtraSelections()
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (m_token_under_cursor.references && !m_token_under_cursor.references->referencesInFile().empty())
  {
    for (const SymbolReferencesInDocument::Position& pos : m_token_under_cursor.references->referencesInFile())
    {
      QTextEdit::ExtraSelection selection;
      selection.format.setBackground(QColor("lightcyan"));
      selection.cursor = QTextCursor(document()->findBlockByNumber(pos.line - 1));
      int blockpos = selection.cursor.block().position();
      selection.cursor.setPosition(blockpos + pos.col - 1, QTextCursor::MoveAnchor);
      selection.cursor.setPosition(blockpos + pos.col - 1 + symbolUnderCursor()->name().length(), QTextCursor::KeepAnchor);
      extraSelections.append(selection);
    }
  }

  setExtraSelections(extraSelections);
}


CodeViewerContextMenuHandler::CodeViewerContextMenuHandler(CodeViewer& viewer) : QObject(&viewer),
  m_viewer(viewer)
{
  connect(&m_viewer, &CodeViewer::contextMenuRequested, this, &CodeViewerContextMenuHandler::onContextMenuRequested);
}

CodeViewer& CodeViewerContextMenuHandler::codeviewer() const
{
  return m_viewer;
}

void CodeViewerContextMenuHandler::onContextMenuRequested(QMenu* menu)
{
  fill(menu);
}
