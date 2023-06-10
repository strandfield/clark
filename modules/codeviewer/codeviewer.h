// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_CODEVIEWER_H
#define CLARK_CODEVIEWER_H

#include <QPlainTextEdit>

class CppSyntaxHighlighter;

class IncludesInFile;
class SymbolObject;
class SymbolReferencesInDocument;
class SemaInfoProvider;

class CodeViewer : public QPlainTextEdit
{
  Q_OBJECT
public:
  CodeViewer(const QString& documentPath, const QString& documentContent, QWidget* parent = nullptr);
  ~CodeViewer() = default;

  QString documentPath() const;

  CppSyntaxHighlighter* syntaxHighlighter() const;
  void setSyntaxHighlighter(CppSyntaxHighlighter* highlighter);

  static QFont courierFont();

  SemaInfoProvider* semaInfoProvider() const;
  void setSemaInfoProvider(SemaInfoProvider* provider);

  SymbolObject* symbolUnderCursor() const;

  void goToLine(int l);

  template<typename T, typename...Args>
  T* addContextMenuHandler(Args&&... args);

Q_SIGNALS:
  void contextMenuRequested(QMenu* menu);
  void symbolUnderCursorChanged();
  void symbolUnderCursorClicked();
  void includeDirectiveClicked(const QString& included_file);

protected:
  void mousePressEvent(QMouseEvent* ev) override;
  void mouseMoveEvent(QMouseEvent* ev) override;
  void contextMenuEvent(QContextMenuEvent* ev) override;

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
  CppSyntaxHighlighter* m_syntax_highlighter = nullptr;
  SemaInfoProvider* m_info_provider = nullptr;
  TokenUnderCursor m_token_under_cursor;
  IncludesInFile* m_includes = nullptr;
};

class CodeViewerContextMenuHandler : public QObject
{
  Q_OBJECT
public:
  explicit CodeViewerContextMenuHandler(CodeViewer& viewer);

  CodeViewer& codeviewer() const;

protected Q_SLOTS:
  void onContextMenuRequested(QMenu* menu);

protected:
  virtual void fill(QMenu* menu) = 0;

private:
  CodeViewer& m_viewer;
};

template<typename T, typename...Args>
inline T* CodeViewer::addContextMenuHandler(Args&&... args)
{
  return new T(std::forward<Args>(args)..., *this);
}

#endif // CLARK_CODEVIEWER_H
