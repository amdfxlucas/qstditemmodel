QT += widgets
#requires(qtConfig(treeview))
CONFIG += c++20


win32:
{

INCLUDEPATH += C:/Qt/6.1.2/msvc2019_64/include
DEPENDPATH += C:/Qt/6.1.2/msvc2019_64/include

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
}

unix:
{

INCLUDEPATH+= "/home/lucas/Qt/6.1.3/gcc_64/include/QtGui/6.1.3"
INCLUDEPATH+="/home/lucas/Qt/6.1.3/gcc_64/include/QtCore/6.1.3"
}


HEADERS     =    aqp.hpp \
   ui_mainwindow.h\
   mainwindow.h\
    qstditemmodel_commands.h \
    qundogroup.h \
    myqtreeview.h \
    path.h \
    qstditem.h \
    qstditemcommands.h \
    qstditemmodel.h \
    qstditemmodel_p.h \
    reference_controller.h \
    scope_tagger.h \
    undostack.h \
    undostack_p.h

SOURCES     = aqp.cpp \
            mainwindow.cpp\
            main.cpp\
              qstditemmodel_commands.cpp \
              qundogroup.cpp \
              myqtreeview.cpp \
              qstditem.cpp \
              qstditemcmd.cpp \
              qstditemmodel.cpp \
              qstditemmodelprivate.cpp \
              qstditemprivate.cpp \
              reference_controller.cpp \
              undostack.cpp
