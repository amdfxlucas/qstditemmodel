#ifndef PATH_H
#define PATH_H

#include <QMetaType>
#include <QPair>

typedef QPair<int, int> PathItem;
typedef QList<PathItem> Path;

Q_DECLARE_METATYPE(PathItem)
Q_DECLARE_METATYPE(Path)
//Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(Path)


#endif // PATH_H
