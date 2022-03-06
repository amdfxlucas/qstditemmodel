

#ifndef QSTDITEM_H
#define QSTDITEM_H

#include "import_export.h"

//#include <QUndoCommand>
#include "undostack.h"

// #include "qstditemmodel_p.h"

#include <QtGui/qtguiglobal.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtGui/qbrush.h>
#include <QtGui/qfont.h>
#include <QtGui/qicon.h>
#include <QtCore/QObject>
#ifndef QT_NO_DATASTREAM
#include <QtCore/qdatastream.h>
#endif

QT_REQUIRE_CONFIG(standarditemmodel);

QT_BEGIN_NAMESPACE

#include "path.h"




using time_point_type = std::chrono::time_point<std::chrono::system_clock,std::chrono::minutes >;
using duration_type = std::chrono::minutes;

Q_DECLARE_METATYPE_IMPL(time_point_type)
Q_DECLARE_METATYPE(duration_type)
QDataStream &operator<<(QDataStream &out, const time_point_type &myObj);
QDataStream &operator>>(QDataStream &in, time_point_type &myObj);
QDataStream &operator<<(QDataStream &out, const duration_type &myObj);
QDataStream &operator>>(QDataStream &in, duration_type &myObj);
class QStdItemData
{
public:
    inline QStdItemData() : role(-1) {}
    inline QStdItemData(int r, const QVariant &v) : role(r), value(v) {}
    inline QStdItemData(const std::pair<const int&, const QVariant&> &p) : role(p.first), value(p.second) {}
    int role;
    QVariant value;
    inline bool operator==(const QStdItemData &other) const { return role == other.role && value == other.value; }
};
Q_DECLARE_TYPEINFO(QStdItemData, Q_RELOCATABLE_TYPE);

class QStdItemModel;


class QStdItemPrivate;




class TEST_LIB_EXPORT QStdItem
{



private:


    class  StdItemCmd;
    class RemoveRowsCmd;
    class RemoveColumnsCmd;
    class InsertRowCmd;
    class InsertColumnCmd;
    class SetRowCountCmd;
    class SetColumnCountCmd;
    class SetModelCmd;
    class SetChildCmd;
    class ClearDataCmd;
    class SetDataCmd;

public:

    // gets the items unique identifier
    unsigned long long uuid()const ;

    QStdItem();
    QStdItem(QStdItemModel*);
    explicit QStdItem(const QString &text);
    QStdItem(const QIcon &icon, const QString &text);
    explicit QStdItem(int rows, int columns = 1);
    virtual ~QStdItem();

    virtual QVariant data(int role = Qt::UserRole + 1) const;
    virtual void multiData(QModelRoleDataSpan roleDataSpan) const;
    virtual void setData(const QVariant &value, int role = Qt::UserRole + 1);
    virtual void setItemData(const QMap<int, QVariant> &roles);
    virtual QMap<int,QVariant> itemData()const;
    void clearData();

    inline QString text() const {
        return qvariant_cast<QString>(data(Qt::DisplayRole));
    }
    inline void setText(const QString &text);

    inline QIcon icon() const {
        return qvariant_cast<QIcon>(data(Qt::DecorationRole));
    }
    inline void setIcon(const QIcon &icon);

    inline QString toolTip() const {
        return qvariant_cast<QString>(data(Qt::ToolTipRole));
    }
    inline void setToolTip(const QString &toolTip);

#ifndef QT_NO_STATUSTIP
    inline QString statusTip() const {
        return qvariant_cast<QString>(data(Qt::StatusTipRole));
    }
    inline void setStatusTip(const QString &statusTip);
#endif

#if QT_CONFIG(whatsthis)
    inline QString whatsThis() const {
        return qvariant_cast<QString>(data(Qt::WhatsThisRole));
    }
    inline void setWhatsThis(const QString &whatsThis);
#endif

    inline QSize sizeHint() const {
        return qvariant_cast<QSize>(data(Qt::SizeHintRole));
    }
    inline void setSizeHint(const QSize &sizeHint);

    inline QFont font() const {
        return qvariant_cast<QFont>(data(Qt::FontRole));
    }
    inline void setFont(const QFont &font);

    inline Qt::Alignment textAlignment() const {
        return Qt::Alignment(qvariant_cast<int>(data(Qt::TextAlignmentRole)));
    }
    inline void setTextAlignment(Qt::Alignment textAlignment);

    inline QBrush background() const {
        return qvariant_cast<QBrush>(data(Qt::BackgroundRole));
    }
    inline void setBackground(const QBrush &brush);

    inline QBrush foreground() const {
        return qvariant_cast<QBrush>(data(Qt::ForegroundRole));
    }
    inline void setForeground(const QBrush &brush);

    inline Qt::CheckState checkState() const {
        return Qt::CheckState(qvariant_cast<int>(data(Qt::CheckStateRole)));
    }
    inline void setCheckState(Qt::CheckState checkState);

    inline QString accessibleText() const {
        return qvariant_cast<QString>(data(Qt::AccessibleTextRole));
    }
    inline void setAccessibleText(const QString &accessibleText);

    inline QString accessibleDescription() const {
        return qvariant_cast<QString>(data(Qt::AccessibleDescriptionRole));
    }
    inline void setAccessibleDescription(const QString &accessibleDescription);

    Qt::ItemFlags flags() const;
    void setFlags(Qt::ItemFlags flags);

    inline bool isEnabled() const {
        return (flags() & Qt::ItemIsEnabled) != 0;
    }
    void setEnabled(bool enabled);

    inline bool isEditable() const {
        return (flags() & Qt::ItemIsEditable) != 0;
    }
    void setEditable(bool editable);

    inline bool isSelectable() const {
        return (flags() & Qt::ItemIsSelectable) != 0;
    }
    void setSelectable(bool selectable);

    inline bool isCheckable() const {
        return (flags() & Qt::ItemIsUserCheckable) != 0;
    }
    void setCheckable(bool checkable);

    inline bool isAutoTristate() const {
        return (flags() & Qt::ItemIsAutoTristate) != 0;
    }
    void setAutoTristate(bool tristate);

    inline bool isUserTristate() const {
        return (flags() & Qt::ItemIsUserTristate) != 0;
    }
    void setUserTristate(bool tristate);

#if QT_CONFIG(draganddrop)
    inline bool isDragEnabled() const {
        return (flags() & Qt::ItemIsDragEnabled) != 0;
    }
    void setDragEnabled(bool dragEnabled);

    inline bool isDropEnabled() const {
        return (flags() & Qt::ItemIsDropEnabled) != 0;
    }
    void setDropEnabled(bool dropEnabled);
#endif // QT_CONFIG(draganddrop)

    QStdItem *parent() const;
    int row() const;
    int column() const;
    QModelIndex index() const;
    QStdItemModel *model() const;

    int rowCount() const;
    void setRowCount(int rows);
    int columnCount() const;
    void setColumnCount(int columns);

    bool hasChild(unsigned long long int uuid)const;

    bool hasChildren() const;
    QStdItem *child(int row, int column = 0) const;
    void setChild(int row, int column, QStdItem *item);
    inline void setChild(int row, QStdItem *item);

    void insertRow(int row, const QList<QStdItem*> &items);
    void insertRows(int row, const QList<QStdItem*> &items);
    void insertRows(int row, int count);
    void insertColumn(int column, const QList<QStdItem*> &items);
    void insertColumns(int column, int count);

    void removeRow(int row);
    void removeColumn(int column);
    void removeRows(int row, int count);
    void removeColumns(int column, int count);

    inline void appendRow(const QList<QStdItem*> &items);
    inline void appendRows(const QList<QStdItem*> &items);
    inline void appendColumn(const QList<QStdItem*> &items);
    inline void insertRow(int row, QStdItem *item);
    inline void appendRow(QStdItem *item);

    QStdItem *takeChild(int row, int column = 0);
    QList<QStdItem*> takeRow(int row);
    QList<QStdItem*> takeColumn(int column);

    void sortChildren(int column, Qt::SortOrder order = Qt::AscendingOrder);

    virtual QStdItem *clone() const;

    enum ItemType { Type = 0, UserType = 1000 };
    virtual int type() const;

#ifndef QT_NO_DATASTREAM
    virtual void read(QDataStream &in);
    virtual void write(QDataStream &out) const;
#endif
    virtual bool operator<(const QStdItem &other) const;

    QStdItem(const QStdItem &other);
    QStdItem(QStdItemPrivate &dd);
     QStdItem(QStdItemPrivate *dd);
    QStdItem &operator=(const QStdItem &other);
        virtual void update();

protected:



    void setModel(QStdItemModel*);
    QScopedPointer<QStdItemPrivate> d_ptr;

    void emitDataChanged();
    Q_DECLARE_PRIVATE(QStdItem)
private:

    friend class QStdItemModelPrivate;
    friend class QStdItemModel;

    friend class reference;
    friend class reference_controller;
};

inline void QStdItem::setText(const QString &atext)
{ setData(atext, Qt::DisplayRole); }

inline void QStdItem::setIcon(const QIcon &aicon)
{ setData(aicon, Qt::DecorationRole); }

inline void QStdItem::setToolTip(const QString &atoolTip)
{ setData(atoolTip, Qt::ToolTipRole); }

#ifndef QT_NO_STATUSTIP
inline void QStdItem::setStatusTip(const QString &astatusTip)
{ setData(astatusTip, Qt::StatusTipRole); }
#endif

#if QT_CONFIG(whatsthis)
inline void QStdItem::setWhatsThis(const QString &awhatsThis)
{ setData(awhatsThis, Qt::WhatsThisRole); }
#endif

inline void QStdItem::setSizeHint(const QSize &asizeHint)
{ setData(asizeHint, Qt::SizeHintRole); }

inline void QStdItem::setFont(const QFont &afont)
{ setData(afont, Qt::FontRole); }

inline void QStdItem::setTextAlignment(Qt::Alignment atextAlignment)
{ setData(int(atextAlignment), Qt::TextAlignmentRole); }

inline void QStdItem::setBackground(const QBrush &abrush)
{ setData(abrush, Qt::BackgroundRole); }

inline void QStdItem::setForeground(const QBrush &abrush)
{ setData(abrush, Qt::ForegroundRole); }

inline void QStdItem::setCheckState(Qt::CheckState acheckState)
{ setData(acheckState, Qt::CheckStateRole); }

inline void QStdItem::setAccessibleText(const QString &aaccessibleText)
{ setData(aaccessibleText, Qt::AccessibleTextRole); }

inline void QStdItem::setAccessibleDescription(const QString &aaccessibleDescription)
{ setData(aaccessibleDescription, Qt::AccessibleDescriptionRole); }

inline void QStdItem::setChild(int arow, QStdItem *aitem)
{ setChild(arow, 0, aitem); }

inline void QStdItem::appendRow(const QList<QStdItem*> &aitems)
{ insertRow(rowCount(), aitems); }

inline void QStdItem::appendRows(const QList<QStdItem*> &aitems)
{ insertRows(rowCount(), aitems); }

inline void QStdItem::appendColumn(const QList<QStdItem*> &aitems)
{ insertColumn(columnCount(), aitems); }

inline void QStdItem::insertRow(int arow, QStdItem *aitem)
{ insertRow(arow, QList<QStdItem*>() << aitem); }

inline void QStdItem::appendRow(QStdItem *aitem)
{ insertRow(rowCount(), aitem); }






#endif // QSTDITEM_H
