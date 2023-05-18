// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "qmethod.h"

#include <QObject>
#include <QMetaObject>

QMethod::QMethod(QObject* obj, const char* method) :
  m_object(obj),
  m_method(method)
{

}

QMethod::~QMethod()
{

}

/**
 * \brief schedules a call to the method
 * 
 * If a call has already been scheduled, this does nothing.
 * 
 * Use clearCallFlag() (in the body of the method) to reset the 'call' flag 
 * so that a new call can be scheduled.
 */
void QMethod::scheduleCall()
{
  if (m_call_scheduled)
    return;

  QMetaObject::invokeMethod(m_object, m_method, Qt::QueuedConnection);

  m_call_scheduled = true;
}

/**
 * \brief clears the call flags so that a new call can be scheduled
 */
void QMethod::clearCallFlag()
{
  m_call_scheduled = false;
}
