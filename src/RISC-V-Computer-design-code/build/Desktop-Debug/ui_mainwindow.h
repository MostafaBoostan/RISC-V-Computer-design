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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
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
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_8;
    QSpacerItem *horizontalSpacer;
    QGridLayout *gridLayout_2;
    QLabel *label_29;
    QLabel *label_9;
    QLabel *funct3_label;
    QLabel *ir_label;
    QLabel *imm_label;
    QLabel *label_22;
    QLabel *funct7_label;
    QLabel *upcode_label;
    QLabel *sc_label;
    QLabel *label_28;
    QLabel *mar_label;
    QLabel *gar_label;
    QLabel *inpr_label;
    QLabel *label_20;
    QLabel *pc_label;
    QLabel *label_18;
    QLabel *label_24;
    QLabel *label_26;
    QLabel *label_16;
    QLabel *outr_label;
    QLabel *label_6;
    QLabel *label_14;
    QSpacerItem *verticalSpacer_4;
    QLabel *label_4;
    QTableView *general_table;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTextEdit *code_field;
    QLabel *label_2;
    QPlainTextEdit *console_field;
    QLabel *label_3;
    QTextEdit *input_field;
    QLabel *label_7;
    QPlainTextEdit *output_field;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_5;
    QTableView *ram_table;
    QHBoxLayout *horizontalLayout;
    QPushButton *compile_btn;
    QPushButton *run_btn;
    QPushButton *reset_btn;
    QPushButton *clock_btn;
    QSpacerItem *verticalSpacer_5;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1048, 744);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayout_3 = new QGridLayout(centralwidget);
        gridLayout_3->setObjectName("gridLayout_3");
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName("label_8");
        label_8->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_8);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout_3->addItem(horizontalSpacer);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName("gridLayout_2");
        label_29 = new QLabel(centralwidget);
        label_29->setObjectName("label_29");
        label_29->setMaximumSize(QSize(60, 16777215));
        label_29->setTextFormat(Qt::MarkdownText);
        label_29->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_29, 10, 0, 1, 1);

        label_9 = new QLabel(centralwidget);
        label_9->setObjectName("label_9");
        label_9->setMaximumSize(QSize(60, 16777215));
        label_9->setTextFormat(Qt::MarkdownText);
        label_9->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_9, 0, 0, 1, 1);

        funct3_label = new QLabel(centralwidget);
        funct3_label->setObjectName("funct3_label");
        funct3_label->setTextFormat(Qt::MarkdownText);
        funct3_label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(funct3_label, 2, 1, 1, 1);

        ir_label = new QLabel(centralwidget);
        ir_label->setObjectName("ir_label");
        ir_label->setTextFormat(Qt::MarkdownText);
        ir_label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(ir_label, 4, 1, 1, 1);

        imm_label = new QLabel(centralwidget);
        imm_label->setObjectName("imm_label");
        imm_label->setTextFormat(Qt::MarkdownText);
        imm_label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(imm_label, 7, 1, 1, 1);

        label_22 = new QLabel(centralwidget);
        label_22->setObjectName("label_22");
        label_22->setMaximumSize(QSize(60, 16777215));
        label_22->setTextFormat(Qt::MarkdownText);
        label_22->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_22, 1, 0, 1, 1);

        funct7_label = new QLabel(centralwidget);
        funct7_label->setObjectName("funct7_label");
        funct7_label->setTextFormat(Qt::MarkdownText);
        funct7_label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(funct7_label, 1, 1, 1, 1);

        upcode_label = new QLabel(centralwidget);
        upcode_label->setObjectName("upcode_label");
        upcode_label->setTextFormat(Qt::MarkdownText);
        upcode_label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(upcode_label, 6, 1, 1, 1);

        sc_label = new QLabel(centralwidget);
        sc_label->setObjectName("sc_label");
        sc_label->setTextFormat(Qt::MarkdownText);
        sc_label->setAlignment(Qt::AlignCenter);
        sc_label->setWordWrap(false);
        sc_label->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

        gridLayout_2->addWidget(sc_label, 8, 1, 1, 1);

        label_28 = new QLabel(centralwidget);
        label_28->setObjectName("label_28");
        label_28->setMaximumSize(QSize(60, 16777215));
        label_28->setTextFormat(Qt::MarkdownText);
        label_28->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_28, 9, 0, 1, 1);

        mar_label = new QLabel(centralwidget);
        mar_label->setObjectName("mar_label");
        mar_label->setMaximumSize(QSize(217, 16777215));
        mar_label->setTextFormat(Qt::MarkdownText);
        mar_label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(mar_label, 5, 1, 1, 1);

        gar_label = new QLabel(centralwidget);
        gar_label->setObjectName("gar_label");
        gar_label->setTextFormat(Qt::MarkdownText);
        gar_label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(gar_label, 3, 1, 1, 1);

        inpr_label = new QLabel(centralwidget);
        inpr_label->setObjectName("inpr_label");
        inpr_label->setTextFormat(Qt::MarkdownText);
        inpr_label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(inpr_label, 9, 1, 1, 1);

        label_20 = new QLabel(centralwidget);
        label_20->setObjectName("label_20");
        label_20->setMaximumSize(QSize(60, 16777215));
        label_20->setTextFormat(Qt::MarkdownText);
        label_20->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_20, 2, 0, 1, 1);

        pc_label = new QLabel(centralwidget);
        pc_label->setObjectName("pc_label");
        pc_label->setTextFormat(Qt::MarkdownText);
        pc_label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(pc_label, 0, 1, 1, 1);

        label_18 = new QLabel(centralwidget);
        label_18->setObjectName("label_18");
        label_18->setMaximumSize(QSize(60, 16777215));
        label_18->setTextFormat(Qt::MarkdownText);
        label_18->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_18, 4, 0, 1, 1);

        label_24 = new QLabel(centralwidget);
        label_24->setObjectName("label_24");
        label_24->setMaximumSize(QSize(60, 16777215));
        label_24->setTextFormat(Qt::MarkdownText);
        label_24->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_24, 6, 0, 1, 1);

        label_26 = new QLabel(centralwidget);
        label_26->setObjectName("label_26");
        label_26->setMaximumSize(QSize(60, 16777215));
        label_26->setTextFormat(Qt::MarkdownText);
        label_26->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_26, 8, 0, 1, 1);

        label_16 = new QLabel(centralwidget);
        label_16->setObjectName("label_16");
        label_16->setMaximumSize(QSize(60, 16777215));
        label_16->setTextFormat(Qt::MarkdownText);
        label_16->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_16, 3, 0, 1, 1);

        outr_label = new QLabel(centralwidget);
        outr_label->setObjectName("outr_label");
        outr_label->setTextFormat(Qt::MarkdownText);
        outr_label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(outr_label, 10, 1, 1, 1);

        label_6 = new QLabel(centralwidget);
        label_6->setObjectName("label_6");
        label_6->setMaximumSize(QSize(60, 16777215));
        label_6->setTextFormat(Qt::MarkdownText);
        label_6->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_6, 5, 0, 1, 1);

        label_14 = new QLabel(centralwidget);
        label_14->setObjectName("label_14");
        label_14->setMaximumSize(QSize(60, 16777215));
        label_14->setTextFormat(Qt::MarkdownText);
        label_14->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_14, 7, 0, 1, 1);


        verticalLayout_3->addLayout(gridLayout_2);

        verticalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        verticalLayout_3->addItem(verticalSpacer_4);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");
        label_4->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_4);

        general_table = new QTableView(centralwidget);
        general_table->setObjectName("general_table");

        verticalLayout_3->addWidget(general_table);


        gridLayout_3->addLayout(verticalLayout_3, 2, 1, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        label = new QLabel(centralwidget);
        label->setObjectName("label");

        verticalLayout->addWidget(label, 0, Qt::AlignHCenter|Qt::AlignVCenter);

        code_field = new QTextEdit(centralwidget);
        code_field->setObjectName("code_field");
        code_field->setMinimumSize(QSize(0, 200));

        verticalLayout->addWidget(code_field);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");

        verticalLayout->addWidget(label_2, 0, Qt::AlignHCenter);

        console_field = new QPlainTextEdit(centralwidget);
        console_field->setObjectName("console_field");
        console_field->setMaximumSize(QSize(16777215, 50));
        console_field->setReadOnly(true);

        verticalLayout->addWidget(console_field);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");

        verticalLayout->addWidget(label_3, 0, Qt::AlignHCenter);

        input_field = new QTextEdit(centralwidget);
        input_field->setObjectName("input_field");
        input_field->setMaximumSize(QSize(16777215, 50));

        verticalLayout->addWidget(input_field);

        label_7 = new QLabel(centralwidget);
        label_7->setObjectName("label_7");

        verticalLayout->addWidget(label_7, 0, Qt::AlignHCenter);

        output_field = new QPlainTextEdit(centralwidget);
        output_field->setObjectName("output_field");
        output_field->setMaximumSize(QSize(16777215, 50));
        output_field->setReadOnly(true);

        verticalLayout->addWidget(output_field);


        gridLayout_3->addLayout(verticalLayout, 2, 0, 1, 1);

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


        gridLayout_3->addLayout(verticalLayout_2, 2, 2, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        compile_btn = new QPushButton(centralwidget);
        compile_btn->setObjectName("compile_btn");
        compile_btn->setMinimumSize(QSize(0, 35));
        compile_btn->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout->addWidget(compile_btn);

        run_btn = new QPushButton(centralwidget);
        run_btn->setObjectName("run_btn");
        run_btn->setMinimumSize(QSize(0, 35));
        run_btn->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout->addWidget(run_btn);

        reset_btn = new QPushButton(centralwidget);
        reset_btn->setObjectName("reset_btn");
        reset_btn->setMinimumSize(QSize(0, 35));
        reset_btn->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout->addWidget(reset_btn);

        clock_btn = new QPushButton(centralwidget);
        clock_btn->setObjectName("clock_btn");
        clock_btn->setMinimumSize(QSize(0, 35));
        clock_btn->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout->addWidget(clock_btn);


        gridLayout_3->addLayout(horizontalLayout, 0, 0, 1, 3);

        verticalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(verticalSpacer_5, 1, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1048, 25));
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
        label_8->setText(QCoreApplication::translate("MainWindow", "Special purpose", nullptr));
        label_29->setText(QCoreApplication::translate("MainWindow", "OUTR", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "PC", nullptr));
        funct3_label->setText(QCoreApplication::translate("MainWindow", "000", nullptr));
        ir_label->setText(QCoreApplication::translate("MainWindow", "00000000000000000000000000000000", nullptr));
        imm_label->setText(QCoreApplication::translate("MainWindow", "\\-", nullptr));
        label_22->setText(QCoreApplication::translate("MainWindow", "FUNCT7", nullptr));
        funct7_label->setText(QCoreApplication::translate("MainWindow", "0000000", nullptr));
        upcode_label->setText(QCoreApplication::translate("MainWindow", "0000000", nullptr));
        sc_label->setText(QCoreApplication::translate("MainWindow", "\\- \\- \\-", nullptr));
        label_28->setText(QCoreApplication::translate("MainWindow", "INPR", nullptr));
        mar_label->setText(QCoreApplication::translate("MainWindow", "0000000000000000", nullptr));
        gar_label->setText(QCoreApplication::translate("MainWindow", "00000", nullptr));
        inpr_label->setText(QCoreApplication::translate("MainWindow", "00000000000000000000000000000000", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "FUNCT3", nullptr));
        pc_label->setText(QCoreApplication::translate("MainWindow", "0001000000000000", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "IR", nullptr));
        label_24->setText(QCoreApplication::translate("MainWindow", "UPCODE", nullptr));
        label_26->setText(QCoreApplication::translate("MainWindow", "SC", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "GAR", nullptr));
        outr_label->setText(QCoreApplication::translate("MainWindow", "00000000000000000000000000000000", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "MAR", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "IMM", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "General purpose", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Code", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", " Console ", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Input", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Output", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "RAM", nullptr));
        compile_btn->setText(QCoreApplication::translate("MainWindow", "Compile", nullptr));
        run_btn->setText(QCoreApplication::translate("MainWindow", "Run", nullptr));
        reset_btn->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
        clock_btn->setText(QCoreApplication::translate("MainWindow", "Clock", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
