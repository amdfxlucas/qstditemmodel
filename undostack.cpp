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

#include <QtCore/qdebug.h>
#include "undostack.h"
#if QT_CONFIG(undogroup)
#include "qundogroup.h"
#endif
#include "undostack_p.h"

QT_BEGIN_NAMESPACE


QUndoCommand::QUndoCommand(const QString &text, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    setText(text);
}

/*!
    Constructs a QUndoCommand object with parent \a parent.

    If \a parent is not \nullptr, this command is appended to parent's
    child list. The parent command then owns this command and will delete
    it in its destructor.

    \sa ~QUndoCommand()
*/

QUndoCommand::QUndoCommand(QUndoCommand *parent)
{
    d = new QUndoCommandPrivate;
    if (parent != nullptr)
        parent->d->child_list.append(this);
}

/*!
    Destroys the QUndoCommand object and all child commands.

    \sa QUndoCommand()
*/

QUndoCommand::~QUndoCommand()
{
    qDeleteAll(d->child_list);
    delete d;
}

/*!
    \since 5.9

    Returns whether the command is obsolete.

    The boolean is used for the automatic removal of commands that are not necessary in the
    stack anymore. The isObsolete function is checked in the functions UndoStack::push(),
    UndoStack::undo(), UndoStack::redo(), and UndoStack::setIndex().

    \sa setObsolete(), mergeWith(), UndoStack::push(), UndoStack::undo(), UndoStack::redo()
*/

bool QUndoCommand::isObsolete() const
{
    return d->obsolete;
}

/*!
    \since 5.9

    Sets whether the command is obsolete to \a obsolete.

    \sa isObsolete(), mergeWith(), UndoStack::push(), UndoStack::undo(), UndoStack::redo()
*/

void QUndoCommand::setObsolete(bool obsolete)
{
    d->obsolete = obsolete;
}

/*!
    Returns the ID of this command.

    A command ID is used in command compression. It must be an integer unique to
    this command's class, or -1 if the command doesn't support compression.

    If the command supports compression this function must be overridden in the
    derived class to return the correct ID. The base implementation returns -1.

    UndoStack::push() will only try to merge two commands if they have the
    same ID, and the ID is not -1.

    \sa mergeWith(), UndoStack::push()
*/

int QUndoCommand::id() const
{
    return -1;
}

/*!
    Attempts to merge this command with \a command. Returns \c true on
    success; otherwise returns \c false.

    If this function returns \c true, calling this command's redo() must have the same
    effect as redoing both this command and \a command.
    Similarly, calling this command's undo() must have the same effect as undoing
    \a command and this command.

    UndoStack will only try to merge two commands if they have the same id, and
    the id is not -1.

    The default implementation returns \c false.

    \snippet code/src_gui_util_qundostack.cpp 3

    \sa id(), UndoStack::push()
*/

bool QUndoCommand::mergeWith(const QUndoCommand *command)
{
    Q_UNUSED(command);
    return false;
}

/*!
    Applies a change to the document. This function must be implemented in
    the derived class. Calling UndoStack::push(),
    UndoStack::undo() or UndoStack::redo() from this function leads to
    undefined beahavior.

    The default implementation calls redo() on all child commands.

    \sa undo()
*/

void QUndoCommand::redo()
{
    for (int i = 0; i < d->child_list.size(); ++i)
        d->child_list.at(i)->redo();
}

/*!
    Reverts a change to the document. After undo() is called, the state of
    the document should be the same as before redo() was called. This function must
    be implemented in the derived class. Calling UndoStack::push(),
    UndoStack::undo() or UndoStack::redo() from this function leads to
    undefined beahavior.

    The default implementation calls undo() on all child commands in reverse order.

    \sa redo()
*/

void QUndoCommand::undo()
{
    for (int i = d->child_list.size() - 1; i >= 0; --i)
        d->child_list.at(i)->undo();
}

/*!
    Returns a short text string describing what this command does; for example,
    "insert text".

    The text is used for names of items in QUndoView.

    \sa actionText(), setText(), UndoStack::createUndoAction(), UndoStack::createRedoAction()
*/

QString QUndoCommand::text() const
{
    return d->text;
}

/*!
    \since 4.8

    Returns a short text string describing what this command does; for example,
    "insert text".

    The text is used when the text properties of the stack's undo and redo
    actions are updated.

    \sa text(), setText(), UndoStack::createUndoAction(), UndoStack::createRedoAction()
*/

QString QUndoCommand::actionText() const
{
    return d->actionText;
}

/*!
    Sets the command's text to be the \a text specified.

    The specified text should be a short user-readable string describing what this
    command does.

    If you need to have two different strings for text() and actionText(), separate
    them with "\\n" and pass into this function. Even if you do not use this feature
    for English strings during development, you can still let translators use two
    different strings in order to match specific languages' needs.
    The described feature and the function actionText() are available since Qt 4.8.

    \sa text(), actionText(), UndoStack::createUndoAction(), UndoStack::createRedoAction()
*/

void QUndoCommand::setText(const QString &text)
{
    int cdpos = text.indexOf(QLatin1Char('\n'));
    if (cdpos > 0) {
        d->text = text.left(cdpos);
        d->actionText = text.mid(cdpos + 1);
    } else {
        d->text = text;
        d->actionText = text;
    }
}

/*!
    \since 4.4

    Returns the number of child commands in this command.

    \sa child()
*/

int QUndoCommand::childCount() const
{
    return d->child_list.count();
}

/*!
    \since 4.4

    Returns the child command at \a index.

    \sa childCount(), UndoStack::command()
*/

const QUndoCommand *QUndoCommand::child(int index) const
{
    if (index < 0 || index >= d->child_list.count())
        return nullptr;
    return d->child_list.at(index);
}

#if QT_CONFIG(undostack)


void UndoStackPrivate::setIndex(int idx, bool clean)
{
    Q_Q(UndoStack);

    bool was_clean = index == clean_index;

    if (idx != index) {
        index = idx;
        emit q->indexChanged(index);
        emit q->canUndoChanged(q->canUndo());
        emit q->undoTextChanged(q->undoText());
        emit q->canRedoChanged(q->canRedo());
        emit q->redoTextChanged(q->redoText());
    }

    if (clean)
        clean_index = index;

    bool is_clean = index == clean_index;
    if (is_clean != was_clean)
        emit q->cleanChanged(is_clean);
}

/*! \internal
    If the number of commands on the stack exceedes the undo limit, deletes commands from
    the bottom of the stack.

    Returns \c true if commands were deleted.
*/

bool UndoStackPrivate::checkUndoLimit()
{
    if (undo_limit <= 0 || !macro_stack.isEmpty() || undo_limit >= command_list.count())
        return false;

    int del_count = command_list.count() - undo_limit;

    for (int i = 0; i < del_count; ++i)
        delete command_list.takeFirst();

    index -= del_count;
    if (clean_index != -1) {
        if (clean_index < del_count)
            clean_index = -1; // we've deleted the clean command
        else
            clean_index -= del_count;
    }

    return true;
}

/*!
    Constructs an empty undo stack with the parent \a parent. The
    stack will initially be in the clean state. If \a parent is a
    QUndoGroup object, the stack is automatically added to the group.

    \sa push()
*/

UndoStack::UndoStack(QObject *parent)
    : QObject(*(new UndoStackPrivate), parent)
{
#if QT_CONFIG(undogroup)
    if (QUndoGroup *group = qobject_cast<QUndoGroup*>(parent))
        group->addStack(this);
#endif
}

/*!
    Destroys the undo stack, deleting any commands that are on it. If the
    stack is in a QUndoGroup, the stack is automatically removed from the group.

    \sa UndoStack()
*/


bool UndoStack::is_locked() const
{ Q_D(const UndoStack);
    return d->push_lock;
}

void UndoStack::lock_push(bool value)
{ Q_D(UndoStack);
    d->push_lock = value;}

UndoStack::~UndoStack()
{
#if QT_CONFIG(undogroup)
    Q_D(UndoStack);
    if (d->group != nullptr)
        d->group->removeStack(this);
#endif
    clear();
}

/*!
    Clears the command stack by deleting all commands on it, and returns the stack
    to the clean state.

    Commands are not undone or redone; the state of the edited object remains
    unchanged.

    This function is usually used when the contents of the document are
    abandoned.

    \sa UndoStack()
*/

void UndoStack::clear()
{
    Q_D(UndoStack);

    if (d->command_list.isEmpty())
        return;

    bool was_clean = isClean();

    d->macro_stack.clear();
    qDeleteAll(d->command_list);
    d->command_list.clear();

    d->index = 0;
    d->clean_index = 0;

    emit indexChanged(0);
    emit canUndoChanged(false);
    emit undoTextChanged(QString());
    emit canRedoChanged(false);
    emit redoTextChanged(QString());

    if (!was_clean)
        emit cleanChanged(true);
}



void UndoStack::push(QUndoCommand *cmd)
{
    Q_D(UndoStack);
    if (!cmd->isObsolete())
        cmd->redo();


 // if the stack is on lock  all subsequently pushed commands are only executed(redone) but not pushed onto the stack
    if(d->push_lock)
    {
        delete cmd; // otherwise it'll leak
        return;
    }

    bool macro = !d->macro_stack.isEmpty();

    QUndoCommand *cur = nullptr;
    if (macro)
    {
        QUndoCommand *macro_cmd = d->macro_stack.constLast();
        if (!macro_cmd->d->child_list.isEmpty())
            cur = macro_cmd->d->child_list.constLast();
    } else
    {
        if (d->index > 0)
            cur = d->command_list.at(d->index - 1);
        while (d->index < d->command_list.size())
            delete d->command_list.takeLast();
        if (d->clean_index > d->index)
            d->clean_index = -1; // we've deleted the clean state
    }

    bool try_merge = cur != nullptr
                        && cur->id() != -1
                        && cur->id() == cmd->id()
                        && (macro || d->index != d->clean_index);

    if (try_merge && cur->mergeWith(cmd)) {
        delete cmd;

        if (macro) {
            if (cur->isObsolete())
                delete d->macro_stack.constLast()->d->child_list.takeLast();
        } else {
            if (cur->isObsolete()) {
                delete d->command_list.takeLast();

                d->setIndex(d->index - 1, false);
            } else {
                emit indexChanged(d->index);
                emit canUndoChanged(canUndo());
                emit undoTextChanged(undoText());
                emit canRedoChanged(canRedo());
                emit redoTextChanged(redoText());
            }
        }
    } else if (cmd->isObsolete()) {
        delete cmd; // command should be deleted and NOT added to the stack
    } else {
        if (macro) {
            d->macro_stack.constLast()->d->child_list.append(cmd);
        } else {
            d->command_list.append(cmd);
            d->checkUndoLimit();
            d->setIndex(d->index + 1, false);
        }
    }
}

/*!
    Marks the stack as clean and emits cleanChanged() if the stack was
    not already clean.

    This is typically called when a document is saved, for example.

    Whenever the stack returns to this state through the use of undo/redo
    commands, it emits the signal cleanChanged(). This signal is also
    emitted when the stack leaves the clean state.

    \sa isClean(), resetClean(), cleanIndex()
*/

void UndoStack::setClean()
{
    Q_D(UndoStack);
    if (Q_UNLIKELY(!d->macro_stack.isEmpty())) {
        qWarning("UndoStack::setClean(): cannot set clean in the middle of a macro");
        return;
    }

    d->setIndex(d->index, true);
}

/*!
    \since 5.8

    Leaves the clean state and emits cleanChanged() if the stack was clean.
    This method resets the clean index to -1.

    This is typically called in the following cases, when a document has been:
    \list
    \li created basing on some template and has not been saved,
        so no filename has been associated with the document yet.
    \li restored from a backup file.
    \li changed outside of the editor and the user did not reload it.
    \endlist

    \sa isClean(), setClean(), cleanIndex()
*/

void UndoStack::resetClean()
{
    Q_D(UndoStack);
    const bool was_clean = isClean();
    d->clean_index = -1;
    if (was_clean)
        emit cleanChanged(false);
}

/*!
    \since 5.12
    \property UndoStack::clean
    \brief the clean status of this stack.

    This property indicates whether or not the stack is clean. For example, a
    stack is clean when a document has been saved.

    \sa isClean(), setClean(), resetClean(), cleanIndex()
*/

/*!
    If the stack is in the clean state, returns \c true; otherwise returns \c false.

    \sa setClean(), cleanIndex()
*/

bool UndoStack::isClean() const
{
    Q_D(const UndoStack);
    if (!d->macro_stack.isEmpty())
        return false;
    return d->clean_index == d->index;
}

/*!
    Returns the clean index. This is the index at which setClean() was called.

    A stack may not have a clean index. This happens if a document is saved,
    some commands are undone, then a new command is pushed. Since
    push() deletes all the undone commands before pushing the new command, the stack
    can't return to the clean state again. In this case, this function returns -1.
    The -1 may also be returned after an explicit call to resetClean().

    \sa isClean(), setClean()
*/

int UndoStack::cleanIndex() const
{
    Q_D(const UndoStack);
    return d->clean_index;
}

/*!
    Undoes the command below the current command by calling QUndoCommand::undo().
    Decrements the current command index.

    If the stack is empty, or if the bottom command on the stack has already been
    undone, this function does nothing.

    After the command is undone, if QUndoCommand::isObsolete() returns \c true,
    then the command will be deleted from the stack. Additionally, if the clean
    index is greater than or equal to the current command index, then the clean
    index is reset.

    \sa redo(), index()
*/

void UndoStack::undo()
{
    Q_D(UndoStack);
    if (d->index == 0)
        return;

    if (Q_UNLIKELY(!d->macro_stack.isEmpty()))
    {
        qWarning("UndoStack::undo(): cannot undo in the middle of a macro");
        return;
    }

    int idx = d->index - 1;
    QUndoCommand *cmd = d->command_list.at(idx);

    if (!cmd->isObsolete())
        cmd->undo();

    if (cmd->isObsolete()) { // A separate check is done b/c the undo command may set obsolete flag
        delete d->command_list.takeAt(idx);

        if (d->clean_index > idx)
            resetClean();
    }

    d->setIndex(idx, false);
}

/*!
    Redoes the current command by calling QUndoCommand::redo(). Increments the current
    command index.

    If the stack is empty, or if the top command on the stack has already been
    redone, this function does nothing.

    If QUndoCommand::isObsolete() returns true for the current command, then
    the command will be deleted from the stack. Additionally, if the clean
    index is greater than or equal to the current command index, then the clean
    index is reset.

    \sa undo(), index()
*/

void UndoStack::redo()
{
    Q_D(UndoStack);
    if (d->index == d->command_list.size())
        return;

    if (Q_UNLIKELY(!d->macro_stack.isEmpty())) {
        qWarning("UndoStack::redo(): cannot redo in the middle of a macro");
        return;
    }

    int idx = d->index;
    QUndoCommand *cmd = d->command_list.at(idx);

    if (!cmd->isObsolete())
        cmd->redo(); // A separate check is done b/c the undo command may set obsolete flag

    if (cmd->isObsolete()) {
        delete d->command_list.takeAt(idx);

        if (d->clean_index > idx)
            resetClean();
    } else {
        d->setIndex(d->index + 1, false);
    }
}

/*!
    Returns the number of commands on the stack. Macro commands are counted as
    one command.

    \sa index(), setIndex(), command()
*/

int UndoStack::count() const
{
    Q_D(const UndoStack);
    return d->command_list.size();
}

/*!
    Returns the index of the current command. This is the command that will be
    executed on the next call to redo(). It is not always the top-most command
    on the stack, since a number of commands may have been undone.

    \sa undo(), redo(), count()
*/

int UndoStack::index() const
{
    Q_D(const UndoStack);
    return d->index;
}

/*!
    Repeatedly calls undo() or redo() until the current command index reaches
    \a idx. This function can be used to roll the state of the document forwards
    of backwards. indexChanged() is emitted only once.

    \sa index(), count(), undo(), redo()
*/

void UndoStack::setIndex(int idx)
{
    Q_D(UndoStack);
    if (Q_UNLIKELY(!d->macro_stack.isEmpty())) {
        qWarning("UndoStack::setIndex(): cannot set index in the middle of a macro");
        return;
    }

    if (idx < 0)
        idx = 0;
    else if (idx > d->command_list.size())
        idx = d->command_list.size();

    int i = d->index;
    while (i < idx) {
        QUndoCommand *cmd = d->command_list.at(i);

        if (!cmd->isObsolete())
            cmd->redo();  // A separate check is done b/c the undo command may set obsolete flag

        if (cmd->isObsolete()) {
            delete d->command_list.takeAt(i);

            if (d->clean_index > i)
                resetClean();

            idx--; // Subtract from idx because we removed a command
        } else {
            i++;
        }
    }

    while (i > idx) {
        QUndoCommand *cmd = d->command_list.at(--i);

        cmd->undo();
        if (cmd->isObsolete()) {
            delete d->command_list.takeAt(i);

            if (d->clean_index > i)
                resetClean();
        }
    }

    d->setIndex(idx, false);
}

/*!
    \since 5.12
    \property UndoStack::canUndo
    \brief whether this stack can undo.

    This property indicates whether or not there is a command that can be
    undone.

    \sa canUndo(), index(), canRedo()
*/

/*!
    Returns \c true if there is a command available for undo; otherwise returns \c false.

    This function returns \c false if the stack is empty, or if the bottom command
    on the stack has already been undone.

    Synonymous with index() == 0.

    \sa index(), canRedo()
*/

bool UndoStack::canUndo() const
{
    Q_D(const UndoStack);
    if (!d->macro_stack.isEmpty())
        return false;
    return d->index > 0;
}

/*!
    \since 5.12
    \property UndoStack::canRedo
    \brief whether this stack can redo.

    This property indicates whether or not there is a command that can be
    redone.

    \sa canRedo(), index(), canUndo()
*/

/*!
    Returns \c true if there is a command available for redo; otherwise returns \c false.

    This function returns \c false if the stack is empty or if the top command
    on the stack has already been redone.

    Synonymous with index() == count().

    \sa index(), canUndo()
*/

bool UndoStack::canRedo() const
{
    Q_D(const UndoStack);
    if (!d->macro_stack.isEmpty())
        return false;
    return d->index < d->command_list.size();
}

/*!
    \since 5.12
    \property UndoStack::undoText
    \brief the undo text of the next command that is undone.

    This property holds the text of the command which will be undone in the
    next call to undo().

    \sa undoText(), QUndoCommand::actionText(), redoText()
*/

/*!
    Returns the text of the command which will be undone in the next call to undo().

    \sa QUndoCommand::actionText(), redoText()
*/

QString UndoStack::undoText() const
{
    Q_D(const UndoStack);
    if (!d->macro_stack.isEmpty())
        return QString();
    if (d->index > 0)
        return d->command_list.at(d->index - 1)->actionText();
    return QString();
}

/*!
    \since 5.12
    \property UndoStack::redoText
    \brief the redo text of the next command that is redone.

    This property holds the text of the command which will be redone in the
    next call to redo().

    \sa redoText(), QUndoCommand::actionText(), undoText()
*/

/*!
    Returns the text of the command which will be redone in the next call to redo().

    \sa QUndoCommand::actionText(), undoText()
*/

QString UndoStack::redoText() const
{
    Q_D(const UndoStack);
    if (!d->macro_stack.isEmpty())
        return QString();
    if (d->index < d->command_list.size())
        return d->command_list.at(d->index)->actionText();
    return QString();
}

#ifndef QT_NO_ACTION

/*!
    \internal

    Sets the text property of \a action to \a text, applying \a prefix, and falling back to \a defaultText if \a text is empty.
*/
void UndoStackPrivate::setPrefixedText(QAction *action, const QString &prefix, const QString &defaultText, const QString &text)
{
    if (defaultText.isEmpty()) {
        QString s = prefix;
        if (!prefix.isEmpty() && !text.isEmpty())
            s.append(QLatin1Char(' '));
        s.append(text);
        action->setText(s);
    } else {
        if (text.isEmpty())
            action->setText(defaultText);
        else
            action->setText(prefix.arg(text));
    }
};

/*!
    Creates an undo QAction object with the given \a parent.

    Triggering this action will cause a call to undo(). The text of this action
    is the text of the command which will be undone in the next call to undo(),
    prefixed by the specified \a prefix. If there is no command available for undo,
    this action will be disabled.

    If \a prefix is empty, the default template "Undo %1" is used instead of prefix.
    Before Qt 4.8, the prefix "Undo" was used by default.

    \sa createRedoAction(), canUndo(), QUndoCommand::text()
*/

QAction *UndoStack::createUndoAction(QObject *parent, const QString &prefix) const
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

    connect(this, &UndoStack::canUndoChanged, action, &QAction::setEnabled);
    connect(this, &UndoStack::undoTextChanged, action, [=](const QString &text) {
        UndoStackPrivate::setPrefixedText(action, effectivePrefix, defaultText, text);
    });
    connect(action, &QAction::triggered, this, &UndoStack::undo);

    return action;
}

/*!
    Creates an redo QAction object with the given \a parent.

    Triggering this action will cause a call to redo(). The text of this action
    is the text of the command which will be redone in the next call to redo(),
    prefixed by the specified \a prefix. If there is no command available for redo,
    this action will be disabled.

    If \a prefix is empty, the default template "Redo %1" is used instead of prefix.
    Before Qt 4.8, the prefix "Redo" was used by default.

    \sa createUndoAction(), canRedo(), QUndoCommand::text()
*/

QAction *UndoStack::createRedoAction(QObject *parent, const QString &prefix) const
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

    connect(this, &UndoStack::canRedoChanged, action, &QAction::setEnabled);
    connect(this, &UndoStack::redoTextChanged, action, [=](const QString &text) {
        UndoStackPrivate::setPrefixedText(action, effectivePrefix, defaultText, text);
    });
    connect(action, &QAction::triggered, this, &UndoStack::redo);

    return action;
}

#endif // QT_NO_ACTION

/*!
    Begins composition of a macro command with the given \a text description.

    An empty command described by the specified \a text is pushed on the stack.
    Any subsequent commands pushed on the stack will be appended to the empty
    command's children until endMacro() is called.

    Calls to beginMacro() and endMacro() may be nested, but every call to
    beginMacro() must have a matching call to endMacro().

    While a macro is being composed, the stack is disabled. This means that:
    \list
    \li indexChanged() and cleanChanged() are not emitted,
    \li canUndo() and canRedo() return false,
    \li calling undo() or redo() has no effect,
    \li the undo/redo actions are disabled.
    \endlist

    The stack becomes enabled and appropriate signals are emitted when endMacro()
    is called for the outermost macro.

    \snippet code/src_gui_util_qundostack.cpp 4

    This code is equivalent to:

    \snippet code/src_gui_util_qundostack.cpp 5

    \sa endMacro()
*/

void UndoStack::beginMacro(const QString &text)
{
    Q_D(UndoStack);
    QUndoCommand *cmd = new QUndoCommand();
    cmd->setText(text);

    if (d->macro_stack.isEmpty())
    {
        while (d->index < d->command_list.size())
            delete d->command_list.takeLast();
        if (d->clean_index > d->index)
            d->clean_index = -1; // we've deleted the clean state
        d->command_list.append(cmd);
    } else
    {
        d->macro_stack.constLast()->d->child_list.append(cmd);
    }
    d->macro_stack.append(cmd);

    if (d->macro_stack.count() == 1) {
        emit canUndoChanged(false);
        emit undoTextChanged(QString());
        emit canRedoChanged(false);
        emit redoTextChanged(QString());
    }
}

/*!
    Ends composition of a macro command.

    If this is the outermost macro in a set nested macros, this function emits
    indexChanged() once for the entire macro command.

    \sa beginMacro()
*/

void UndoStack::endMacro()
{
    Q_D(UndoStack);
    if (Q_UNLIKELY(d->macro_stack.isEmpty())) {
        qWarning("UndoStack::endMacro(): no matching beginMacro()");
        return;
    }

    d->macro_stack.removeLast();

    if (d->macro_stack.isEmpty()) {
        d->checkUndoLimit();
        d->setIndex(d->index + 1, false);
    }
}

/*!
  \since 4.4

  Returns a const pointer to the command at \a index.

  This function returns a const pointer, because modifying a command,
  once it has been pushed onto the stack and executed, almost always
  causes corruption of the state of the document, if the command is
  later undone or redone.

  \sa QUndoCommand::child()
*/
const QUndoCommand *UndoStack::command(int index) const
{
    Q_D(const UndoStack);

    if (index < 0 || index >= d->command_list.count())
        return nullptr;
    return d->command_list.at(index);
}

/*!
    Returns the text of the command at index \a idx.

    \sa beginMacro()
*/

QString UndoStack::text(int idx) const
{
    Q_D(const UndoStack);

    if (idx < 0 || idx >= d->command_list.size())
        return QString();
    return d->command_list.at(idx)->text();
}

/*!
    \property UndoStack::undoLimit
    \brief the maximum number of commands on this stack.
    \since 4.3

    When the number of commands on a stack exceedes the stack's undoLimit, commands are
    deleted from the bottom of the stack. Macro commands (commands with child commands)
    are treated as one command. The default value is 0, which means that there is no
    limit.

    This property may only be set when the undo stack is empty, since setting it on a
    non-empty stack might delete the command at the current index. Calling setUndoLimit()
    on a non-empty stack prints a warning and does nothing.
*/

void UndoStack::setUndoLimit(int limit)
{
    Q_D(UndoStack);

    if (Q_UNLIKELY(!d->command_list.isEmpty())) {
        qWarning("UndoStack::setUndoLimit(): an undo limit can only be set when the stack is empty");
        return;
    }

    if (limit == d->undo_limit)
        return;
    d->undo_limit = limit;
    d->checkUndoLimit();
}

int UndoStack::undoLimit() const
{
    Q_D(const UndoStack);

    return d->undo_limit;
}

/*!
    \property UndoStack::active
    \brief the active status of this stack.

    An application often has multiple undo stacks, one for each opened document. The active
    stack is the one associated with the currently active document. If the stack belongs
    to a QUndoGroup, calls to QUndoGroup::undo() or QUndoGroup::redo() will be forwarded
    to this stack when it is active. If the QUndoGroup is watched by a QUndoView, the view
    will display the contents of this stack when it is active. If the stack does not belong to
    a QUndoGroup, making it active has no effect.

    It is the programmer's responsibility to specify which stack is active by
    calling setActive(), usually when the associated document window receives focus.

    \sa QUndoGroup
*/

void UndoStack::setActive(bool active)
{
#if !QT_CONFIG(undogroup)
    Q_UNUSED(active);
#else
    Q_D(UndoStack);

    if (d->group != nullptr) {
        if (active)
            d->group->setActiveStack(this);
        else if (d->group->activeStack() == this)
            d->group->setActiveStack(nullptr);
    }
#endif
}

bool UndoStack::isActive() const
{
#if !QT_CONFIG(undogroup)
    return true;
#else
    Q_D(const UndoStack);
    return d->group == nullptr || d->group->activeStack() == this;
#endif
}


QT_END_NAMESPACE

#include "moc_undostack.cpp"

#endif // QT_CONFIG(undostack)
