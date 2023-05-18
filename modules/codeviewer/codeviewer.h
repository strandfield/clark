// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_CODEVIEWER_H
#define CLARK_CODEVIEWER_H

#include <QPlainTextEdit>

class SyntaxHighlighter;

class IncludesInFile;
class SymbolObject;
class SymbolReferencesInDocument;
class SymbolInfoProvider;

class CodeViewer : public QPlainTextEdit
{
  Q_OBJECT
public:
  CodeViewer(const QString& documentPath, const QString& documentContent, QWidget* parent = nullptr);
  ~CodeViewer() = default;

  QString documentPath() const;

  SyntaxHighlighter* syntaxHighlighter() const;

  static QFont courierFont();

  SymbolInfoProvider* symbolInfoProvider() const;
  void setSymbolInfoProvider(SymbolInfoProvider* provider);

  SymbolObject* symbolUnderCursor() const;

  void goToLine(int l);
  
Q_SIGNALS:
  void symbolUnderCursorChanged();
  void symbolUnderCursorClicked();
  void includeDirectiveClicked(const QString& included_file);

protected:
  void mousePressEvent(QMouseEvent* ev) override;
  void mouseMoveEvent(QMouseEvent* ev) override;

protected:
  void updateTokenUnderCursor(const QTextCursor& cursor);
  void setTokenUnderCursor(int line, int col, int span, SymbolObject* sym);
  void setTokenUnderCursor(int line, int col, int span, QString includedFile);
  void clearTokenUnderCursor();
  void setSymbolUnderCursor(SymbolObject* sym);
  void onSymbolUnderCursorChanged();
  void onSymbolUnderCursorInfoAvailable();
  void setIncludedFileUnderCursor(const QString& filePath);
  void onIncludeUnderCursorChanged();
  void clearIncludes();
  void fetchIncludes();
  void refreshExtraSelections();

private:
  struct TokenUnderCursor
  {
    int line = -1;
    int col = -1;
    int span = 0;
    SymbolObject* symbol = nullptr;
    SymbolReferencesInDocument* references = nullptr;
    QString included_file;
  };

private:
  SyntaxHighlighter* m_syntax_highlighter = nullptr;
  SymbolInfoProvider* m_info_provider = nullptr;
  TokenUnderCursor m_token_under_cursor;
  IncludesInFile* m_includes = nullptr;
};

#endif // CLARK_CODEVIEWER_H
