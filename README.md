# QStdItemModel
improved QStandardItemModel with:
- inbuild UndoRedo support,
- Cut/Paste of Items,
- save/load from/to file

Have you ever felt the need, to let users of your application undo the changes,
they inflict on ItemView's underlying models ,
through their interaction with'em ?

Provide'em with undo/redo MenuActions, just as they're used to from their everyday word processor ?

With a framework as mature as Qt this should be all that easy !

Yet i found that it still takes some lengths...

What you find in this repo is my solution for this matter.
An implementation of Qt's QAbstract/Standard-ItemModel-Interface,
in which every single public modifying method can seamlessly be un/re-done ,
by a mere call to QStdItemModel::undo/redo.

 In its main branch this repo contains an example project similar to [Qt's Simple-Tree-Model](https://doc.qt.io/qt-5/qtwidgets-itemviews-editabletreemodel-example.html), that uses QStdItemModel instead.
 A QUndoView adjacent to the ItemView reveals any QUndoCommands pushed onto the model's internal UndoStack,
 as a consequence of user actions carried out in the GUI.
 
 If your interest is (self)educational in nature you might want to have a look at [^fn2],[^fn1] which i found helpful.
 As opposed to [^fn2] which proposes a 'model-based' undo design[^fn3],
 i pursue an 'item-based' undo design where the model's public modifying interface functions degrade into mere QUndoStackMacros (without own logic).
 
 If you want to use the new model in your own project, checkout the `shared-lib` branch and build the library
 (which you can then link your project against, just as you do with any other Qt-lib).
 Because all exported classes have undergone a slight renaming from their Qt-counterparts,
 linking against the library and normal qt simultaneously, does not cause any symbol clashes.
 
 
 [^fn1]: [Advanced Qt Programming by Mark Summerfield - Chapter 4  Model/View Tree Models -timelog](https://github.com/jhj/aqp-qt5/tree/master/timelog2)
 [^fn2]: [Qt Quaterly Issue 25 Q1 2008 - Undo/Redo with Item Views- by Witold Wysota](https://doc.qt.io/archives/qq/qq25-undo.html)
[^fn3]: That is to say the logic which implements the undo/redo functionality resides in the model's methods itsselfes.


moreover it implements:
-   [`bool	moveRow(const QModelIndex &sourceParent, int sourceRow, const QModelIndex &destinationParent, int destinationChild)`](https://doc.qt.io/qt-6/qabstractitemmodel.html#moveRow)
-   [`virtual bool	moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)`](https://doc.qt.io/qt-6/qabstractitemmodel.html#moveRows)
