// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_TOKENIZER_H
#define CLARK_TOKENIZER_H

#include <cpptok/tokenizer.h>

#include <QString>

#include <string>
#include <vector>

class QStringLineTokenizer
{
public:
  QString qstring;
  std::string text;
  std::vector<int> offsets;
  cpptok::Tokenizer lexer;

  void tokenize(const QString& qstr)
  {
    qstring = qstr;

    lexer.output.clear();

    {
      offsets.clear();
      text.clear();

      text = qstr.toStdString();

      int offset = 0;

      for (int i(0); i < qstr.size(); ++i)
      {
        QChar c = qstr.at(i);

        int codepoint = c.unicode();

        if (c.isSurrogate())
        {
          ++i;
          codepoint = c.isLowSurrogate() ? QChar::surrogateToUcs4(qstr.at(i), c) : QChar::surrogateToUcs4(c, qstr.at(i));
        }

        if (codepoint <= 0x007F)
        {
          offsets.push_back(offset);
        }
        else if (codepoint <= 0x07FF)
        {
          offsets.push_back(offset);
          offsets.push_back(offset);
        }
        else if (codepoint <= 0xFFFF)
        {
          offsets.push_back(offset);
          offsets.push_back(offset);
          offsets.push_back(offset);
        }
        else
        {
          offsets.push_back(offset);
          offsets.push_back(offset);
          offsets.push_back(offset);
          offsets.push_back(offset);
        }

        offset += c.isSurrogate() ? 2 : 1;
      }

      offsets.push_back(offset);
    }

    lexer.tokenize(text);
  }

  int offset(const cpptok::Token& tok) const
  {
    size_t off = tok.text().data() - text.data();
    return offsets.at(off);
  }

  int length(const cpptok::Token& tok) const
  {

    size_t off = tok.text().data() - text.data();
    off += tok.text().length();
    return offsets.at(off) - offset(tok);
  }

  int col(const cpptok::Token& tok) const
  {
    return std::distance(text.data(), tok.text().data()) + 1;
  }

  std::vector<cpptok::Token>::const_iterator tokenAt(int col) const
  {
    for (auto it = lexer.output.begin(); it != lexer.output.end(); ++it)
    {
      if (this->col(*it) <= col && this->col(*it) + it->text().length() > col)
        return it;
    }

    return lexer.output.end();
  }
};

#endif // CLARK_TOKENIZER_H
