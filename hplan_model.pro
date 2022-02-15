QT += widgets
requires(qtConfig(treeview))


RC_ICONS = altro-debolon-squarelogo.ico# altro_debolon.ico

# FORMS       =
HEADERS     = mainwindow.h \
              hplan_item.h \
              hplan_model.h \
              myqtreeview.h \
              plan_model_builder.h \
              treeitem.h \
              treemodel.h \
              ui_mainwindow.h\
              _qabstractitemmodel.h\
              _qabstractitemmodel_p.h\
             # _qstandarditemmodel.h\
             # _qstandarditemmodel_p.h


RESOURCES   = editabletreemodel.qrc

SOURCES     = mainwindow.cpp \
              hplan_item.cpp \
              hplan_model.cpp \
              myqtreeview.cpp \
              plan_model_builder.cpp \
              treeitem.cpp \
              treemodel.cpp \
              main.cpp\
              #../qt6_src/_qabstractitemmodel.cpp\
              #../qt6_src/_qstandarditemmodel.cpp



#INCLUDEPATH+= "J:\qt6_src"

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/widgets/itemviews/editabletreemodel
#INSTALLS += target
