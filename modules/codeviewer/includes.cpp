// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "includes.h"

#include <algorithm>

IncludesInFile::IncludesInFile(const QString& filePath, QObject* parent) : SemaObject(parent),
  m_filepath(filePath)
{

}

IncludesInFile::~IncludesInFile()
{

}

const QString& IncludesInFile::filePath() const
{
  return m_filepath;
}

const std::vector<IncludesInFile::Include>& IncludesInFile::includesInFile() const
{
  return m_includes;
}

void IncludesInFile::setIncludesInFile(std::vector<Include> incls)
{
  m_includes = std::move(incls);
  Q_EMIT changed();
}

void IncludesInFile::addIncludesInFile(const std::vector<Include>& incls)
{
  if (incls.empty())
    return;

  m_includes.insert(m_includes.end(), incls.begin(), incls.end());
  Q_EMIT changed();
}

bool IncludesInFile::hasInclude(int line, QString* included_file) const
{
  auto it = std::find_if(m_includes.begin(), m_includes.end(), [line](const Include& incl) {
    return incl.line == line;
    });

  if (included_file && it != m_includes.end())
    *included_file = it->included_file;

  return it != m_includes.end();
}

QString IncludesInFile::getInclude(int line) const
{
  QString inc;
  hasInclude(line, &inc);
  return inc;
}
