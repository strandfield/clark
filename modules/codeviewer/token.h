// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_TOKEN_H
#define CLARK_TOKEN_H

#include <cpptok/token.h>

class QTextDocument;

struct TokenInfo
{
  const QTextDocument* document = nullptr;
  int line = -1;
  int column = -1;
  cpptok::Token token;
};

#endif // CLARK_TOKEN_H
