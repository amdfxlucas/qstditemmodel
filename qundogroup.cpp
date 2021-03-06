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

#include "qundogroup.h"
#include "undostack.h"
#include "undostack_p.h"

QT_BEGIN_NAMESPACE

class QUndoGroupPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QUndoGroup)
public:
    QUndoGroupPrivate() : active(nullptr) {}

    UndoStack *active;
    QList<UndoStack*> stack_list;
};

/*!
    Creates an empty QUndoGroup object with parent \a parent.

    \sa addStack()
*/

QUndoGroup::QUndoGroup(QObject *parent)
    : QObject(*new QUndoGroupPrivate(), parent)
{
}

/*!
    Destroys the QUndoGroup.
*/
QUndoGroup::~QUndoGroup()
{
    // Ensure all UndoStacks no longer refer to this group.
    Q_D(QUndoGroup);
    QList<UndoStack *>::iterator it = d->stack_list.begin();
    QList<UndoStack *>::iterator end = d->stack_list.end();
    while (it != end) {
        (*it)->d_func()->group = nullptr;
        ++it;
    }
}

/*!
    Adds \a stack to this group. The group does not take ownership of the stack. Another
    way of adding a stack to a group is by specifying the group as the stack's parent
    QObject in UndoStack::UndoStack(). In this case, the stack is deleted when the
    group is deleted, in the usual manner of QObjects.

    \sa removeStack(), stacks(), UndoStack::UndoStack()
*/

void QUndoGroup::addStack(UndoStack *stack)
{
    Q_D(QUndoGroup);

    if (d->stack_list.contains(stack))
        return;
    d->stack_list.append(stack);

    if (QUndoGroup *other = stack->d_func()->group)
        other->removeStack(stack);
    stack->d_func()->group = this;
}

/*!
    Removes \a stack from this group. If the stack was the active stack in the group,
    the active stack becomes 0.

    \sa addStack(), stacks(), UndoStack::~UndoStack()
*/

void QUndoGroup::removeStack(UndoStack *stack)
{
    Q_D(QUndoGroup);

    if (d->stack_list.removeAll(stack) == 0)
        return;
    if (stack == d->active)
        setActiveStack(nullptr);
    stack->d_func()->group = nullptr;
}

/*!
    Returns a list of stacks in this group.

    \sa addStack(), removeStack()
*/

QList<UndoStack*> QUndoGroup::stacks() const
{
    Q_D(const QUndoGroup);
    return d->stack_list;
}

/*!
    Sets the active stack of this group to \a stack.

    If the stack is not a member of this group, this function does nothing.

    Synonymous with calling UndoStack::setActive() on \a stack.

    The actions returned by createUndoAction() and createRedoAction() will now behave
    in the same way as those returned by \a stack's UndoStack::createUndoAction()
    and UndoStack::createRedoAction().

    \sa UndoStack::setActive(), activeStack()
*/

void QUndoGroup::setActiveStack(UndoStack *stack)
{
    Q_D(QUndoGroup);
    if (d->active == stack)
        return;

    if (d->active != nullptr) {
        disconnect(d->active, SIGNAL(canUndoChanged(bool)),
                    this, SIGNAL(canUndoChanged(bool)));
        disconnect(d->active, SIGNAL(undoTextChanged(QString)),
                    this, SIGNAL(undoTextChanged(QString)));
        disconnect(d->active, SIGNAL(canRedoChanged(bool)),
                    this, SIGNAL(canRedoChanged(bool)));
        disconnect(d->active, SIGNAL(redoTextChanged(QString)),
                    this, SIGNAL(redoTextChanged(QString)));
        disconnect(d->active, SIGNAL(indexChanged(int)),
                    this, SIGNAL(indexChanged(int)));
        disconnect(d->active, SIGNAL(cleanChanged(bool)),
                    this, SIGNAL(cleanChanged(bool)));
    }

    d->active = stack;

    if (d->active == nullptr) {
        emit canUndoChanged(false);
        emit undoTextChanged(QString());
        emit canRedoChanged(false);
        emit redoTextChanged(QString());
        emit cleanChanged(true);
        emit indexChanged(0);
    } else {
        connect(d->active, SIGNAL(canUndoChanged(bool)),
                this, SIGNAL(canUndoChanged(bool)));
        connect(d->active, SIGNAL(undoTextChanged(QString)),
                this, SIGNAL(undoTextChanged(QString)));
        connect(d->active, SIGNAL(canRedoChanged(bool)),
                this, SIGNAL(canRedoChanged(bool)));
        connect(d->active, SIGNAL(redoTextChanged(QString)),
                this, SIGNAL(redoTextChanged(QString)));
        connect(d->active, SIGNAL(indexChanged(int)),
                this, SIGNAL(indexChanged(int)));
        connect(d->active, SIGNAL(cleanChanged(bool)),
                this, SIGNAL(cleanChanged(bool)));
        emit canUndoChanged(d->active->canUndo());
        emit undoTextChanged(d->active->undoText());
        emit canRedoChanged(d->active->canRedo());
        emit redoTextChanged(d->active->redoText());
        emit cleanChanged(d->active->isClean());
        emit indexChanged(d->active->index());
    }

    emit activeStackChanged(d->active);
}

/*!
    Returns the active stack of this group.

    If none of the stacks are active, or if the group is empty, this function
    returns \nullptr.

    \sa setActiveStack(), UndoStack::setActive()
*/

UndoStack *QUndoGroup::activeStack() const
{
    Q_D(const QUndoGroup);
    return d->active;
}

#ifndef QT_NO_ACTION

/*!
    Creates an undo QAction object with parent \a parent.

    Triggering this action will cause a call to UndoStack::undo() on the active stack.
    The text of this action will always be the text of the command which will be undone
    in the next call to undo(), prefixed by \a prefix. If there is no command available
    for undo, if the group is empty or if none of the stacks are active, this action will
    be disabled.

    If \a prefix is empty, the default template "Undo %1" is used instead of prefix.
    Before Qt 4.8, the prefix "Undo" was used by default.

    \sa createRedoAction(), canUndo(), QUndoCommand::text()
*/

QAction *QUndoGroup::createUndoAction(QObject *parent, const QString &prefix) const
{
    QAction *action = new QAction(parent);
    action->setEnabled(canUndo());

    QString effectivePrefix = prefix;
    QString defaultText;
    if (prefix.isEmpty()) {
        effectivePrefix = tr("Undo %1");
        defaultText = tr("Undo", "Default text for undo action");
    }

    UndoStackPrivate::setPrefixedText(action, effectivePrefix, defaultText, undoText());

    connect(this, &QUndoGroup::canUndoChanged, action, &QAction::setEnabled);
    connect(this, &QUndoGroup::undoTextChanged, action, [=](const QString &text) {
        UndoStackPrivate::setPrefixedText(action, effectivePrefix, defaultText, text);
    });
    connect(action, &QAction::triggered, this, &QUndoGroup::undo);

    return action;
}

/*!
    Creates an redo QAction object with parent \a parent.

    Triggering this action will cause a call to UndoStack::redo() on the active stack.
    The text of this action will always be the text of the command which will be redone
    in the next call to redo(), prefixed by \a prefix. If there is no command available
    for redo, if the group is empty or if none of the stacks are active, this action will
    be disabled.

    If \a prefix is empty, the default template "Redo %1" is used instead of prefix.
    Before Qt 4.8, the prefix "Redo" was used by default.

    \sa createUndoAction(), canRedo(), QUndoCommand::text()
*/

QAction *QUndoGroup::createRedoAction(QObject *parent, const QString &prefix) const
{
    QAction *action = new QAction(parent);
    action->setEnabled(canRedo());

    QString effectivePrefix = prefix;
    QString defaultText;
    if (prefix.isEmpty()) {
        effectivePrefix = tr("Redo %1");
        defaultText = tr("Redo", "Default text for redo action");
    }

    UndoStackPrivate::setPrefixedText(action, effectivePrefix, defaultText, redoText());

    connect(this, &QUndoGroup::canRedoChanged, action, &QAction::setEnabled);
    connect(this, &QUndoGroup::redoTextChanged, action, [=](const QString &text) {
        UndoStackPrivate::setPrefixedText(action, effectivePrefix, defaultText, text);
    });
    connect(action, &QAction::triggered, this, &QUndoGroup::redo);
    return action;
}

#endif // QT_NO_ACTION

/*!
    Calls UndoStack::undo() on the active stack.

    If none of the stacks are active, or if the group is empty, this function
    does nothing.

    \sa redo(), canUndo(), setActiveStack()
*/

void QUndoGroup::undo()
{
    Q_D(QUndoGroup);
    if (d->active != nullptr)
        d->active->undo();
}

/*!
    Calls UndoStack::redo() on the active stack.

    If none of the stacks are active, or if the group is empty, this function
    does nothing.

    \sa undo(), canRedo(), setActiveStack()
*/


void QUndoGroup::redo()
{
    Q_D(QUndoGroup);
    if (d->active != nullptr)
        d->active->redo();
}

/*!
    Returns the value of the active stack's UndoStack::canUndo().

    If none of the stacks are active, or if the group is empty, this function
    returns \c false.

    \sa canRedo(), setActiveStack()
*/

bool QUndoGroup::canUndo() const
{
    Q_D(const QUndoGroup);
    return d->active != nullptr && d->active->canUndo();
}

/*!
    Returns the value of the active stack's UndoStack::canRedo().

    If none of the stacks are active, or if the group is empty, this function
    returns \c false.

    \sa canUndo(), setActiveStack()
*/

bool QUndoGroup::canRedo() const
{
    Q_D(const QUndoGroup);
    return d->active != nullptr && d->active->canRedo();
}

/*!
    Returns the value of the active stack's UndoStack::undoText().

    If none of the stacks are active, or if the group is empty, this function
    returns an empty string.

    \sa redoText(), setActiveStack()
*/

QString QUndoGroup::undoText() const
{
    Q_D(const QUndoGroup);
    return d->active == nullptr ? QString() : d->active->undoText();
}

/*!
    Returns the value of the active stack's UndoStack::redoText().

    If none of the stacks are active, or if the group is empty, this function
    returns an empty string.

    \sa undoText(), setActiveStack()
*/

QString QUndoGroup::redoText() const
{
    Q_D(const QUndoGroup);
    return d->active == nullptr ? QString() : d->active->redoText();
}

/*!
    Returns the value of the active stack's UndoStack::isClean().

    If none of the stacks are active, or if the group is empty, this function
    returns \c true.

    \sa setActiveStack()
*/

bool QUndoGroup::isClean() const
{
    Q_D(const QUndoGroup);
    return d->active == nullptr || d->active->isClean();
}

/*! \fn void QUndoGroup::activeStackChanged(UndoStack *stack)

    This signal is emitted whenever the active stack of the group changes. This can happen
    when setActiveStack() or UndoStack::setActive() is called, or when the active stack
    is removed form the group. \a stack is the new active stack. If no stack is active,
    \a stack is 0.

    \sa setActiveStack(), UndoStack::setActive()
*/

/*! \fn void QUndoGroup::indexChanged(int idx)

    This signal is emitted whenever the active stack emits UndoStack::indexChanged()
    or the active stack changes.

    \a idx is the new current index, or 0 if the active stack is 0.

    \sa UndoStack::indexChanged(), setActiveStack()
*/

/*! \fn void QUndoGroup::cleanChanged(bool clean)

    This signal is emitted whenever the active stack emits UndoStack::cleanChanged()
    or the active stack changes.

    \a clean is the new state, or true if the active stack is 0.

    \sa UndoStack::cleanChanged(), setActiveStack()
*/

/*! \fn void QUndoGroup::canUndoChanged(bool canUndo)

    This signal is emitted whenever the active stack emits UndoStack::canUndoChanged()
    or the active stack changes.

    \a canUndo is the new state, or false if the active stack is 0.

    \sa UndoStack::canUndoChanged(), setActiveStack()
*/

/*! \fn void QUndoGroup::canRedoChanged(bool canRedo)

    This signal is emitted whenever the active stack emits UndoStack::canRedoChanged()
    or the active stack changes.

    \a canRedo is the new state, or false if the active stack is 0.

    \sa UndoStack::canRedoChanged(), setActiveStack()
*/

/*! \fn void QUndoGroup::undoTextChanged(const QString &undoText)

    This signal is emitted whenever the active stack emits UndoStack::undoTextChanged()
    or the active stack changes.

    \a undoText is the new state, or an empty string if the active stack is 0.

    \sa UndoStack::undoTextChanged(), setActiveStack()
*/

/*! \fn void QUndoGroup::redoTextChanged(const QString &redoText)

    This signal is emitted whenever the active stack emits UndoStack::redoTextChanged()
    or the active stack changes.

    \a redoText is the new state, or an empty string if the active stack is 0.

    \sa UndoStack::redoTextChanged(), setActiveStack()
*/

QT_END_NAMESPACE

#include "moc_qundogroup.cpp"
