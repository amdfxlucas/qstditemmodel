/********************************************************************************
** Form generated from reading UI file 'moverowsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.1.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOVEROWSDIALOG_H
#define UI_MOVEROWSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MoveRowsDialog
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBoxSource;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *lineEdit_count;
    QLabel *label_2;
    QLineEdit *lineEdit_src_row;
    QLabel *label_3;
    QLineEdit *lineEdit_src_parent_uid;
    QGroupBox *groupBoxDestination;
    QWidget *formLayoutWidget_2;
    QFormLayout *formLayout_2;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *lineEdit_dest_child;
    QLineEdit *lineEdit_dest_parent_uid;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *MoveRowsDialog)
    {
        if (MoveRowsDialog->objectName().isEmpty())
            MoveRowsDialog->setObjectName(QString::fromUtf8("MoveRowsDialog"));
        MoveRowsDialog->resize(270, 300);
        verticalLayoutWidget = new QWidget(MoveRowsDialog);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(20, 0, 241, 291));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        groupBoxSource = new QGroupBox(verticalLayoutWidget);
        groupBoxSource->setObjectName(QString::fromUtf8("groupBoxSource"));
        formLayoutWidget = new QWidget(groupBoxSource);
        formLayoutWidget->setObjectName(QString::fromUtf8("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(40, 30, 191, 89));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(formLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        lineEdit_count = new QLineEdit(formLayoutWidget);
        lineEdit_count->setObjectName(QString::fromUtf8("lineEdit_count"));
        lineEdit_count->setToolTip("how many rows shall be moved ");

        formLayout->setWidget(0, QFormLayout::FieldRole, lineEdit_count);

        label_2 = new QLabel(formLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        lineEdit_src_row = new QLineEdit(formLayoutWidget);
        lineEdit_src_row->setObjectName(QString::fromUtf8("lineEdit_src_row"));

        formLayout->setWidget(1, QFormLayout::FieldRole, lineEdit_src_row);

        label_3 = new QLabel(formLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        lineEdit_src_parent_uid = new QLineEdit(formLayoutWidget);
        lineEdit_src_parent_uid->setObjectName(QString::fromUtf8("lineEdit_src_parent_uid"));
        lineEdit_src_parent_uid->setToolTip("hover the mouse over the desired item in the view and its uid will be displayed as a tooltip");

        formLayout->setWidget(2, QFormLayout::FieldRole, lineEdit_src_parent_uid);


        verticalLayout->addWidget(groupBoxSource);

        groupBoxDestination = new QGroupBox(verticalLayoutWidget);
        groupBoxDestination->setObjectName(QString::fromUtf8("groupBoxDestination"));
        formLayoutWidget_2 = new QWidget(groupBoxDestination);
        formLayoutWidget_2->setObjectName(QString::fromUtf8("formLayoutWidget_2"));
        formLayoutWidget_2->setGeometry(QRect(10, 30, 222, 80));
        formLayout_2 = new QFormLayout(formLayoutWidget_2);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(formLayoutWidget_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_4);

        label_5 = new QLabel(formLayoutWidget_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_5);

        lineEdit_dest_child = new QLineEdit(formLayoutWidget_2);
        lineEdit_dest_child->setObjectName(QString::fromUtf8("lineEdit_dest_child"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, lineEdit_dest_child);

        lineEdit_dest_parent_uid = new QLineEdit(formLayoutWidget_2);
        lineEdit_dest_parent_uid->setObjectName(QString::fromUtf8("lineEdit_dest_parent_uid"));
        lineEdit_dest_parent_uid->setToolTip("hover the mouse over the desired item in the view and its uid will be displayed as a tooltip");

        formLayout_2->setWidget(1, QFormLayout::FieldRole, lineEdit_dest_parent_uid);


        verticalLayout->addWidget(groupBoxDestination);

        buttonBox = new QDialogButtonBox(verticalLayoutWidget);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(MoveRowsDialog);
      //  QObject::connect(buttonBox, &QDialogButtonBox::accepted, MoveRowsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, MoveRowsDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(MoveRowsDialog);
    } // setupUi

    void retranslateUi(QDialog *MoveRowsDialog)
    {
        MoveRowsDialog->setWindowTitle(QCoreApplication::translate("MoveRowsDialog", "MoveRowsDialog", nullptr));
#if QT_CONFIG(tooltip)
        MoveRowsDialog->setToolTip(QCoreApplication::translate("MoveRowsDialog", "here you can enter which rows you want to move and where to", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
        MoveRowsDialog->setWhatsThis(QString());
#endif // QT_CONFIG(whatsthis)
        groupBoxSource->setTitle(QCoreApplication::translate("MoveRowsDialog", "source", nullptr));
        label->setText(QCoreApplication::translate("MoveRowsDialog", "count:", nullptr));
        label_2->setText(QCoreApplication::translate("MoveRowsDialog", "source row:", nullptr));
        label_3->setText(QCoreApplication::translate("MoveRowsDialog", "source parent uid:", nullptr));
        groupBoxDestination->setTitle(QCoreApplication::translate("MoveRowsDialog", "destination", nullptr));
        label_4->setText(QCoreApplication::translate("MoveRowsDialog", "destination child:", nullptr));
        label_5->setText(QCoreApplication::translate("MoveRowsDialog", "destination parent uid:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MoveRowsDialog: public Ui_MoveRowsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOVEROWSDIALOG_H
