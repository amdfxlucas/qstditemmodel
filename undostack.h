/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QUNDOSTACK_H
#define QUNDOSTACK_H


#include <QtGui/qtguiglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>


//#include <QUndoGroup>

QT_REQUIRE_CONFIG(undocommand);

QT_BEGIN_NAMESPACE

class QAction;
class QUndoCommandPrivate;
class UndoStackPrivate;

// QUndoCommand symbols are already defined in Qt6Gui.dll / Qt6Guid.dll
// eighter change name from QUndoCommand in UndoCommand everywhere or
// simply do not export QUndoCommand symbols
// THIS IS ONLY POSSIBLE AS LONG AS I DO NOT CHANGE ANYTHING IN THE QUndoCommand
// IMPLEMENTATION !

class /* TEST_LIB_EXPORT*/ QUndoCommand
{
    QUndoCommandPrivate *d;

public:
    explicit QUndoCommand(QUndoCommand *parent = nullptr);
    explicit QUndoCommand(const QString &text, QUndoCommand *parent = nullptr);
    virtual ~QUndoCommand();

    virtual void undo();
    virtual void redo();

    QString text() const;
    QString actionText() const;
    void setText(const QString &text);

    bool isObsolete() const;
    void setObsolete(bool obsolete);

    virtual int id() const;
    virtual bool mergeWith(const QUndoCommand *other);

    int childCount() const;
    const QUndoCommand *child(int index) const;

private:
    Q_DISABLE_COPY(QUndoCommand)
    friend class UndoStack;
};

#if QT_CONFIG(undostack)

class  UndoStack
        : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(UndoStack)
    Q_PROPERTY(bool active READ isActive WRITE setActive)
    Q_PROPERTY(int undoLimit READ undoLimit WRITE setUndoLimit)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(QString undoText READ undoText NOTIFY undoTextChanged)
    Q_PROPERTY(QString redoText READ redoText NOTIFY redoTextChanged)
    Q_PROPERTY(bool clean READ isClean NOTIFY cleanChanged)

public:
    explicit UndoStack(QObject *parent = nullptr);
    ~UndoStack();
    void clear();

    void push(QUndoCommand *cmd);

    // lucas 05.02.22
    bool is_locked() const ;//{return push_lock;}
    void lock_push(bool value);// {push_lock=value;}

   //  bool redoSuccessFlag()const{return  the redoSuccessFlag of the last pushed command   }


    bool canUndo() const;
    bool canRedo() const;
    QString undoText() const;
    QString redoText() const;

    int count() const;
    int index() const;
    QString text(int idx) const;

#ifndef QT_NO_ACTION
    QAction *createUndoAction(QObject *parent, const QString &prefix = QString()) const;
    QAction *createRedoAction(QObject *parent, const QString &prefix = QString()) const;
#endif // QT_NO_ACTION

    bool isActive() const;
    bool isClean() const;
    int cleanIndex() const;

    void beginMacro(const QString &text);
    void endMacro();

    void setUndoLimit(int limit);
    int undoLimit() const;

    const QUndoCommand *command(int index) const;

public Q_SLOTS:
    void setClean();
    void resetClean();
    void setIndex(int idx);
    void undo();
    void redo();
    void setActive(bool active = true);

Q_SIGNALS:
    void indexChanged(int idx);
    void cleanChanged(bool clean);
    void canUndoChanged(bool canUndo);
    void canRedoChanged(bool canRedo);
    void undoTextChanged(const QString &undoText);
    void redoTextChanged(const QString &redoText);

private:
    Q_DISABLE_COPY(UndoStack)
    friend class QUndoGroup;
  //  bool push_lock{false}; // moved to undostack_private to not break binary compatibility
};

class  UndoStackLock
{
private:
    bool prev_state;
    UndoStack* stack{nullptr};
    bool active{false};
public:
    UndoStackLock(UndoStack* s)
    {
        if(s)
        {

        stack=s;
        prev_state=s->is_locked();

        // if the stack is unlocked, lock it
        if(!prev_state)
        { s->lock_push(true);}
        }

    }

    ~UndoStackLock()
    {
        if(stack)
        {
        // if we were the ones who locked the stack
        // unlock it
        if(stack->is_locked() != prev_state)
        {stack->lock_push(prev_state);}
        // we dont want to mistakenly release someone else' lock on the stack
        }

    }
};

#endif // QT_CONFIG(undostack)

QT_END_NAMESPACE

#endif // QUNDOSTACK_H
