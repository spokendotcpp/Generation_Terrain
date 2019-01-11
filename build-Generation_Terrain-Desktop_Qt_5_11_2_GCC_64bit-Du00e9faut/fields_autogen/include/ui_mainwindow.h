/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "include/meshviewerwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionLoad_OBJ;
    QAction *actionExit;
    QAction *actionLaplace_Beltrami;
    QAction *action_smooth;
    QAction *action_flat;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *left_layout;
    MeshViewerWidget *viewer;
    QGroupBox *groupBox;
    QLabel *label_size;
    QSpinBox *spinBox_size;
    QPushButton *regen_button;
    QMenuBar *menubar;
    QMenu *menu_render;
    QMenu *menu_smooth;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(863, 600);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(600, 500));
        MainWindow->setAnimated(true);
        actionLoad_OBJ = new QAction(MainWindow);
        actionLoad_OBJ->setObjectName(QStringLiteral("actionLoad_OBJ"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionLaplace_Beltrami = new QAction(MainWindow);
        actionLaplace_Beltrami->setObjectName(QStringLiteral("actionLaplace_Beltrami"));
        actionLaplace_Beltrami->setEnabled(false);
        action_smooth = new QAction(MainWindow);
        action_smooth->setObjectName(QStringLiteral("action_smooth"));
        action_flat = new QAction(MainWindow);
        action_flat->setObjectName(QStringLiteral("action_flat"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        left_layout = new QVBoxLayout();
        left_layout->setObjectName(QStringLiteral("left_layout"));
        viewer = new MeshViewerWidget(centralWidget);
        viewer->setObjectName(QStringLiteral("viewer"));
        viewer->setFocusPolicy(Qt::StrongFocus);

        left_layout->addWidget(viewer);


        horizontalLayout->addLayout(left_layout);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        label_size = new QLabel(groupBox);
        label_size->setObjectName(QStringLiteral("label_size"));
        label_size->setGeometry(QRect(10, 30, 275, 31));
        spinBox_size = new QSpinBox(groupBox);
        spinBox_size->setObjectName(QStringLiteral("spinBox_size"));
        spinBox_size->setGeometry(QRect(0, 60, 275, 26));
        spinBox_size->setMinimum(1);
        spinBox_size->setMaximum(15);
        spinBox_size->setValue(8);
        regen_button = new QPushButton(groupBox);
        regen_button->setObjectName(QStringLiteral("regen_button"));
        regen_button->setGeometry(QRect(10, 100, 89, 25));

        horizontalLayout->addWidget(groupBox);

        MainWindow->setCentralWidget(centralWidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 863, 22));
        menu_render = new QMenu(menubar);
        menu_render->setObjectName(QStringLiteral("menu_render"));
        menu_smooth = new QMenu(menu_render);
        menu_smooth->setObjectName(QStringLiteral("menu_smooth"));
        MainWindow->setMenuBar(menubar);

        menubar->addAction(menu_render->menuAction());
        menu_render->addAction(menu_smooth->menuAction());
        menu_smooth->addAction(action_smooth);
        menu_smooth->addAction(action_flat);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        actionLoad_OBJ->setText(QApplication::translate("MainWindow", "&Load OBJ", nullptr));
        actionExit->setText(QApplication::translate("MainWindow", "&Exit", nullptr));
        actionLaplace_Beltrami->setText(QApplication::translate("MainWindow", "&Laplace-Beltrami", nullptr));
        action_smooth->setText(QApplication::translate("MainWindow", "&Active", nullptr));
        action_flat->setText(QApplication::translate("MainWindow", "&Flat", nullptr));
        groupBox->setTitle(QString());
        label_size->setText(QApplication::translate("MainWindow", "size of map", nullptr));
        regen_button->setText(QApplication::translate("MainWindow", "g\303\251n\303\251rer", nullptr));
        menu_render->setTitle(QApplication::translate("MainWindow", "Re&nder", nullptr));
        menu_smooth->setTitle(QApplication::translate("MainWindow", "&Smooth", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
