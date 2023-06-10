// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "semaobject.h"

SemaObject::SemaObject(QObject* parent) : QObject(parent)
{

}

bool SemaObject::isComplete() const
{
  return m_complete;
}

void SemaObject::setComplete(bool c)
{
  if (m_complete != c)
  {
    m_complete = c;

    Q_EMIT completeChanged();

    if (c)
      Q_EMIT completed();
  }
}
