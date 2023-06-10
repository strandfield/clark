// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_SEMAOOBJECT_H
#define CLARK_SEMAOOBJECT_H

#include <QObject>

/**
 * \brief base class for all classes providing semantic information about a C++ program
 * 
 * Since computing semantic information can be expensive, it may be done asynchronously.
 * This class provides a isComplete() method to access if the information provided 
 * by the SemaObject is complete.
 */
class SemaObject : public QObject
{
  Q_OBJECT
 
  Q_PROPERTY(bool isComplete READ isComplete WRITE setComplete NOTIFY completeChanged)
public:
  explicit SemaObject(QObject* parent = nullptr);

  bool isComplete() const;
  void setComplete(bool c = true);

Q_SIGNALS:
  void completeChanged();
  void completed();

private:
  bool m_complete = false;
};

#endif // CLARK_SEMAOOBJECT_H
