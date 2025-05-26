/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QPushButton *compile_btn;
    QPushButton *run_btn;
    QPushButton *reset_btn;
    QPushButton *clock_btn;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTextEdit *code_field;
    QLabel *label_2;
    QPlainTextEdit *console_field;
    QLabel *label_3;
    QTextEdit *input_field;
    QLabel *label_7;
    QPlainTextEdit *output_field;
    QFormLayout *formLayout;
    QLabel *label_6;
    QLabel *mar_label;
    QLabel *label_9;
    QLabel *pc_label;
    QLabel *label_14;
    QLabel *imm_label;
    QLabel *label_16;
    QLabel *gar_label;
    QLabel *label_18;
    QLabel *ir_label;
    QLabel *label_20;
    QLabel *funct3_label;
    QLabel *label_22;
    QLabel *funct7_label;
    QLabel *label_24;
    QLabel *upcode_label;
    QLabel *label_26;
    QLabel *sc_label;
    QLabel *label_28;
    QLabel *inpr_label;
    QLabel *label_29;
    QLabel *outr_label;
    QTableView *tableView;
    QLabel *label_4;
    QLabel *label_8;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_5;
    QTableView *ram_table;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 574);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName("gridLayout");
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        compile_btn = new QPushButton(centralwidget);
        compile_btn->setObjectName("compile_btn");

        horizontalLayout->addWidget(compile_btn);

        run_btn = new QPushButton(centralwidget);
        run_btn->setObjectName("run_btn");

        horizontalLayout->addWidget(run_btn);

        reset_btn = new QPushButton(centralwidget);
        reset_btn->setObjectName("reset_btn");

        horizontalLayout->addWidget(reset_btn);

        clock_btn = new QPushButton(centralwidget);
        clock_btn->setObjectName("clock_btn");

        horizontalLayout->addWidget(clock_btn);


        verticalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        label = new QLabel(centralwidget);
        label->setObjectName("label");

        verticalLayout->addWidget(label, 0, Qt::AlignHCenter|Qt::AlignVCenter);

        code_field = new QTextEdit(centralwidget);
        code_field->setObjectName("code_field");

        verticalLayout->addWidget(code_field);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");

        verticalLayout->addWidget(label_2, 0, Qt::AlignHCenter);

        console_field = new QPlainTextEdit(centralwidget);
        console_field->setObjectName("console_field");
        console_field->setReadOnly(true);

        verticalLayout->addWidget(console_field);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");

        verticalLayout->addWidget(label_3, 0, Qt::AlignHCenter);

        input_field = new QTextEdit(centralwidget);
        input_field->setObjectName("input_field");

        verticalLayout->addWidget(input_field);

        label_7 = new QLabel(centralwidget);
        label_7->setObjectName("label_7");

        verticalLayout->addWidget(label_7, 0, Qt::AlignHCenter);

        output_field = new QPlainTextEdit(centralwidget);
        output_field->setObjectName("output_field");
        output_field->setReadOnly(true);

        verticalLayout->addWidget(output_field);


        horizontalLayout_2->addLayout(verticalLayout);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName("label_6");

        formLayout->setWidget(0, QFormLayout::LabelRole, label_6);

        mar_label = new QLabel(centralwidget);
        mar_label->setObjectName("mar_label");
        mar_label->setMaximumSize(QSize(217, 16777215));

        formLayout->setWidget(0, QFormLayout::FieldRole, mar_label);

        label_9 = new QLabel(centralwidget);
        label_9->setObjectName("label_9");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_9);

        pc_label = new QLabel(centralwidget);
        pc_label->setObjectName("pc_label");

        formLayout->setWidget(1, QFormLayout::FieldRole, pc_label);

        label_14 = new QLabel(centralwidget);
        label_14->setObjectName("label_14");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_14);

        imm_label = new QLabel(centralwidget);
        imm_label->setObjectName("imm_label");

        formLayout->setWidget(2, QFormLayout::FieldRole, imm_label);

        label_16 = new QLabel(centralwidget);
        label_16->setObjectName("label_16");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_16);

        gar_label = new QLabel(centralwidget);
        gar_label->setObjectName("gar_label");

        formLayout->setWidget(3, QFormLayout::FieldRole, gar_label);

        label_18 = new QLabel(centralwidget);
        label_18->setObjectName("label_18");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_18);

        ir_label = new QLabel(centralwidget);
        ir_label->setObjectName("ir_label");

        formLayout->setWidget(4, QFormLayout::FieldRole, ir_label);

        label_20 = new QLabel(centralwidget);
        label_20->setObjectName("label_20");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_20);

        funct3_label = new QLabel(centralwidget);
        funct3_label->setObjectName("funct3_label");

        formLayout->setWidget(5, QFormLayout::FieldRole, funct3_label);

        label_22 = new QLabel(centralwidget);
        label_22->setObjectName("label_22");

        formLayout->setWidget(6, QFormLayout::LabelRole, label_22);

        funct7_label = new QLabel(centralwidget);
        funct7_label->setObjectName("funct7_label");

        formLayout->setWidget(6, QFormLayout::FieldRole, funct7_label);

        label_24 = new QLabel(centralwidget);
        label_24->setObjectName("label_24");

        formLayout->setWidget(7, QFormLayout::LabelRole, label_24);

        upcode_label = new QLabel(centralwidget);
        upcode_label->setObjectName("upcode_label");

        formLayout->setWidget(7, QFormLayout::FieldRole, upcode_label);

        label_26 = new QLabel(centralwidget);
        label_26->setObjectName("label_26");

        formLayout->setWidget(8, QFormLayout::LabelRole, label_26);

        sc_label = new QLabel(centralwidget);
        sc_label->setObjectName("sc_label");

        formLayout->setWidget(8, QFormLayout::FieldRole, sc_label);

        label_28 = new QLabel(centralwidget);
        label_28->setObjectName("label_28");

        formLayout->setWidget(9, QFormLayout::LabelRole, label_28);

        inpr_label = new QLabel(centralwidget);
        inpr_label->setObjectName("inpr_label");

        formLayout->setWidget(9, QFormLayout::FieldRole, inpr_label);

        label_29 = new QLabel(centralwidget);
        label_29->setObjectName("label_29");

        formLayout->setWidget(10, QFormLayout::LabelRole, label_29);

        outr_label = new QLabel(centralwidget);
        outr_label->setObjectName("outr_label");

        formLayout->setWidget(10, QFormLayout::FieldRole, outr_label);

        tableView = new QTableView(centralwidget);
        tableView->setObjectName("tableView");

        formLayout->setWidget(12, QFormLayout::SpanningRole, tableView);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");

        formLayout->setWidget(11, QFormLayout::LabelRole, label_4);

        label_8 = new QLabel(centralwidget);
        label_8->setObjectName("label_8");

        formLayout->setWidget(11, QFormLayout::FieldRole, label_8);


        horizontalLayout_2->addLayout(formLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName("label_5");

        verticalLayout_2->addWidget(label_5, 0, Qt::AlignHCenter);

        ram_table = new QTableView(centralwidget);
        ram_table->setObjectName("ram_table");
        ram_table->setTabletTracking(false);
        ram_table->horizontalHeader()->setCascadingSectionResizes(true);

        verticalLayout_2->addWidget(ram_table);


        horizontalLayout_2->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout_2);


        gridLayout->addLayout(verticalLayout_3, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 17));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        compile_btn->setText(QCoreApplication::translate("MainWindow", "compile", nullptr));
        run_btn->setText(QCoreApplication::translate("MainWindow", "run", nullptr));
        reset_btn->setText(QCoreApplication::translate("MainWindow", "reset", nullptr));
        clock_btn->setText(QCoreApplication::translate("MainWindow", "clock", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "code", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", " console ", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "input", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "output", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "MAR", nullptr));
        mar_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "PC", nullptr));
        pc_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "IMM", nullptr));
        imm_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "GAR", nullptr));
        gar_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "IR", nullptr));
        ir_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "FUNCT3", nullptr));
        funct3_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_22->setText(QCoreApplication::translate("MainWindow", "FUNCT7", nullptr));
        funct7_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_24->setText(QCoreApplication::translate("MainWindow", "UPCODE", nullptr));
        upcode_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_26->setText(QCoreApplication::translate("MainWindow", "SC", nullptr));
        sc_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_28->setText(QCoreApplication::translate("MainWindow", "INPR", nullptr));
        inpr_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_29->setText(QCoreApplication::translate("MainWindow", "OUTR", nullptr));
        outr_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "generall purpose", nullptr));
        label_8->setText(QString());
        label_5->setText(QCoreApplication::translate("MainWindow", "RAM", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
