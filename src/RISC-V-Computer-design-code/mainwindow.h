#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <QString>
#include <QStandardItem>
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_compile_btn_clicked();

    void on_run_btn_clicked();

    void on_clock_btn_clicked();

    void on_reset_btn_clicked();

    void on_stop_btn_clicked();

    void on_load_btn_clicked();

private:
    Ui::MainWindow *ui;
    bool isCompiling = false;

    int SC_n = 0;
    int stopflag;

    struct InstrInfo {
        std::string funct7;
        std::string rs2;
        std::string rs1;
        std::string funct3;
        std::string rd;
        std::string opcode;
    };

    std:: unordered_map<std::string, InstrInfo> instrMap;
    std::unordered_map<std::string, int> regMap;
    std::unordered_map<std::string, int> symbolTable;
    int registers[32] = {0};
    std::vector<uint8_t> memory;
    int currentAddress = 0;
    size_t maxAddress = 0;
    std::vector<uint8_t> memoryArray;

    bool isNumeric(const std::string& str);
    std::string to_bin(int val, int bits);
    std::vector<std::string> tokenize(const std::string& line);
    std::string assemble(const std::string& line, int& currentPC);
    void parseLabels();
    void storeInMemory(int value, int size, int& address);
    int upper(long long imm);
    int lower(long long imm);
    void saveToBinaryFile(const std::string& filename);
    int binaryToSignedInt(const QString& binStr);

protected:
    void closeEvent(QCloseEvent *event) override;

};
#endif // MAINWINDOW_H
