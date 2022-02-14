QT += widgets
requires(qtConfig(treeview))
CONFIG += c++20

INCLUDEPATH+= "C:\Qt\6.1.2\msvc2019_64\include\QtCore\6.1.2"
INCLUDEPATH+="C:\Qt\6.1.2\msvc2019_64\include\QtWidgets\6.1.2"
INCLUDEPATH+="C:\Qt\6.1.2\msvc2019_64\include\QtGui\6.1.2"
INCLUDEPATH+="C:\Qt\6.1.2\msvc2019_64\include\QtGui\6.1.2"

INCLUDEPATH +="C:\Qt\6.1.2\msvc2019_64\include\QtWidgets\6.1.2\QtWidgets\private"
INCLUDEPATH +="C:\Qt\6.1.2\msvc2019_64\include\QtWidgets\6.1.2\QtWidgets"
INCLUDEPATH +="C:\Qt\6.1.2\msvc2019_64\include\QtCore\6.1.2\QtCore\private"
INCLUDEPATH +="C:\Qt\6.1.2\msvc2019_64\include\QtCore\6.1.2\QtCore"
INCLUDEPATH += "C:\Qt\6.1.2\msvc2019_64\include\QtGui\6.1.2\QtGui\qpa"
INCLUDEPATH += "C:\Qt\6.1.2\msvc2019_64\include\QtGui\6.1.2\QtGui"

# FORMS       = mainwindow.ui
HEADERS     = mainwindow.h \
    aqp.hpp \
    qundogroup.h \
    myqtreeview.h \
    path.h \
    qstditem.h \
    qstditemcommands.h \
    qstditemmodel.h \
    qstditemmodel_p.h \
    reference_controller.h \
    scope_tagger.h \
    ui_mainwindow.h \
    undostack.h \
    undostack_p.h
#RESOURCES   = editabletreemodel.qrc
SOURCES     = mainwindow.cpp \
              aqp.cpp \
              qundogroup.cpp \
              main.cpp \
              myqtreeview.cpp \
              qstditem.cpp \
              qstditemcmd.cpp \
              qstditemmodel.cpp \
              qstditemmodelprivate.cpp \
              qstditemprivate.cpp \
              reference_controller.cpp \
              undostack.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/itemviews/editabletreemodel
INSTALLS += target



INCLUDEPATH += C:/Qt/6.1.2/msvc2019_64/include
DEPENDPATH += C:/Qt/6.1.2/msvc2019_64/include
