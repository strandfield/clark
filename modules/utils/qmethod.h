// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_QMETHOD_H
#define CLARK_QMETHOD_H

class QObject;

/**
 * \brief helper class for scheduling the invokation of a Qt slot
 */
class QMethod
{
private:
  QObject* m_object;
  const char* m_method;
  bool m_call_scheduled = false;

public:
  QMethod(QObject* obj, const char* method);
  ~QMethod();

  void scheduleCall();
  void clearCallFlag();
};

#endif // CLARK_QMETHOD_H
