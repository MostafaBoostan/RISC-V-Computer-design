#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <unordered_map>
#include <vector>
#include <string>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_compile_btn_clicked();

private:
    Ui::MainWindow *ui;

    struct InstrInfo {
        std::string funct7;  // 7 بیت
        std::string rs2;     // 5 بیت
        std::string rs1;     // 5 بیت
        std::string funct3;  // 3 بیت
        std::string rd;      // 5 بیت
        std::string opcode;  // 7 بیت
    };

    std::unordered_map<std::string, InstrInfo> instrMap;
    std::unordered_map<std::string, int> regMap;
    int registers[32] = {0};

    std::string to_bin(int val, int bits);
    std::vector<std::string> tokenize(const std::string& line);
    std::string assemble(const std::string& line);
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    instrMap = {
        {"add", {"0000000", "00000", "00000", "000", "00000", "0110011"}},
        {"sub", {"0100000", "00000", "00000", "000", "00000", "0110011"}},
        {"xor", {"0000000", "00000", "00000", "100", "00000", "0110011"}},
        {"or",  {"0000000", "00000", "00000", "110", "00000", "0110011"}},
        {"and", {"0000000", "00000", "00000", "111", "00000", "0110011"}},
        {"sll", {"0000000", "00000", "00000", "001", "00000", "0110011"}},
        {"srl", {"0000000", "00000", "00000", "101", "00000", "0110011"}},
        {"sra", {"0100000", "00000", "00000", "101", "00000", "0110011"}},
        {"slt", {"0000000", "00000", "00000", "010", "00000", "0110011"}},
        {"sltu",{"0000000", "00000", "00000", "011", "00000", "0110011"}},

        {"addi", {"0000000", "00000", "00000", "000", "00000", "0010011"}}, // imm[11:0]
        {"lh",   {"0000000", "00000", "00000", "001", "00000", "0000011"}},
        {"lw",   {"0000000", "00000", "00000", "010", "00000", "0000011"}},
        {"sh",   {"0000000", "00000", "00000", "001", "00000", "0100011"}},
        {"sw",   {"0000000", "00000", "00000", "010", "00000", "0100011"}},
        {"beq",  {"0000000", "00000", "00000", "000", "00000", "1100011"}},
        {"bne",  {"0000000", "00000", "00000", "001", "00000", "1100011"}},
        {"blt",  {"0000000", "00000", "00000", "100", "00000", "1100011"}},
        {"bge",  {"0000000", "00000", "00000", "101", "00000", "1100011"}},
        {"bltu", {"0000000", "00000", "00000", "110", "00000", "1100011"}},
        {"bgeu", {"0000000", "00000", "00000", "111", "00000", "1100011"}},
        {"jal",  {"00000000000000000000", "00000", "00000", "000", "00000", "1101111"}},
        {"jalr", {"0000000", "00000", "00000", "000", "00000", "1100117"}},
        {"lui",  {"00000000000000000000", "00000", "00000", "000", "00000", "0110111"}},
        {"auipc",{"00000000000000000000", "00000", "00000", "000", "00000", "0010111"}},
        {"mul",  {"0000001", "00000", "00000", "000", "00000", "0110011"}},
        {"mulh", {"0000001", "00000", "00000", "001", "00000", "0110011"}},
        {"div",  {"0000001", "00000", "00000", "100", "00000", "0110011"}},
        {"rem",  {"0000001", "00000", "00000", "110", "00000", "0110011"}}
    };

    regMap = {
        {"x0", 0}, {"x1", 1}, {"x2", 2}, {"x3", 3}, {"x4", 4},
        {"x5", 5}, {"x6", 6}, {"x7", 7}, {"x8", 8}, {"x9", 9},
        {"x10", 10}, {"x11", 11}, {"x12", 12}, {"x13", 13}, {"x14", 14},
        {"x15", 15}, {"x16", 16}, {"x17", 17}, {"x18", 18}, {"x19", 19},
        {"x20", 20}, {"x21", 21}, {"x22", 22}, {"x23", 23}, {"x24", 24},
        {"x25", 25}, {"x26", 26}, {"x27", 27}, {"x28", 28}, {"x29", 29},
        {"x30", 30}, {"x31", 31}
    };

    connect(ui->compile_btn, &QPushButton::clicked, this, &MainWindow::on_compile_btn_clicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_compile_btn_clicked() {
    QString fullText = ui->code_field->toPlainText();
    QStringList lines = fullText.split("\n", Qt::SkipEmptyParts);
    QString output;

    for (const QString& line : lines) {
        std::string binaryInstruction = assemble(line.toStdString());
        if (!binaryInstruction.empty()) {
            output += QString::fromStdString(binaryInstruction) + "\n";
            qDebug() << "Output for line:" << line << QString::fromStdString(binaryInstruction);
        } else {
            qDebug() << "No output for line:" << line;
        }
    }

    if (ui->output_field) {
        ui->output_field->setPlainText(output);
    } else {
        QMessageBox::information(this, "Assembly Output", output);
    }
}

std::string MainWindow::to_bin(int val, int bits) {
    std::string res(bits, '0');
    unsigned int uval = static_cast<unsigned int>(val);
    for (int i = bits - 1; i >= 0; --i) {
        res[i] = (uval & 1) ? '1' : '0';
        uval >>= 1;
    }
    qDebug() << "to_bin(" << val << ", " << bits << "):" << QString::fromStdString(res);
    return res;
}

std::vector<std::string> MainWindow::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    bool inParentheses = false;

    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];

        if (c == '(') {
            inParentheses = true;
            token += c;
            continue;
        } else if (c == ')') {
            inParentheses = false;
            token += c;
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            continue;
        }

        if ((c == ' ' || c == ',') && !inParentheses) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }


    for (std::string& t : tokens) {
        if (!t.empty() && t.back() == ',') {
            t.pop_back();
        }
    }

    qDebug() << "Tokens for line:" << QString::fromStdString(line) << tokens.size() << tokens;
    return tokens;
}

std::string MainWindow::assemble(const std::string& line) {
    std::string binaryInstruction;
    std::vector<std::string> tokens = tokenize(line);
    if (tokens.empty()) {
        qDebug() << "Empty tokens for line:" << QString::fromStdString(line);
        return binaryInstruction;
    }

    std::string inst = tokens[0];
    if (instrMap.find(inst) == instrMap.end()) {
        qDebug() << "Instruction not found:" << QString::fromStdString(inst);
        return binaryInstruction;
    }

    InstrInfo info = instrMap[inst];

    try {
        if (inst == "add" || inst == "sub" || inst == "xor" || inst == "or" || inst == "and" ||
            inst == "sll" || inst == "srl" || inst == "sra" || inst == "slt" || inst == "sltu" ||
            inst == "mul" || inst == "mulh" || inst == "div" || inst == "rem") {
            if (tokens.size() != 4) {
                qDebug() << "Invalid token count for R-type:" << tokens.size() << tokens;
                return binaryInstruction;
            }
            int rd = regMap.at(tokens[1]);
            int rs1 = regMap.at(tokens[2]);
            int rs2 = regMap.at(tokens[3]);

            std::string rd_val = to_bin(rd, 5);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rs2_val = to_bin(rs2, 5);

            binaryInstruction = info.funct7 + rs2_val + rs1_val + info.funct3 + rd_val + info.opcode;
        }
        else if (inst == "addi" || inst == "lh" || inst == "lw" || inst == "jalr") {
            if (tokens.size() != 3) {
                qDebug() << "Invalid token count for I-type:" << tokens.size() << tokens;
                return binaryInstruction;
            }
            int rd = regMap.at(tokens[1]);
            std::string imm_rs1 = tokens[2];
            int imm, rs1;


            size_t pos = imm_rs1.find('(');
            if (pos == std::string::npos || pos == 0 || imm_rs1.back() != ')') {
                qDebug() << "Invalid imm(rs1) format:" << QString::fromStdString(imm_rs1);
                return binaryInstruction;
            }
            std::string imm_str = imm_rs1.substr(0, pos);
            imm = std::stoi(imm_str);
            std::string rs1_str = imm_rs1.substr(pos + 1, imm_rs1.length() - pos - 2);
            qDebug() << "Extracted: imm_str=" << QString::fromStdString(imm_str)
                     << " imm=" << imm
                     << " rs1_str=" << QString::fromStdString(rs1_str);
            if (regMap.find(rs1_str) == regMap.end()) {
                qDebug() << "Invalid register:" << QString::fromStdString(rs1_str);
                return binaryInstruction;
            }
            rs1 = regMap.at(rs1_str);

            std::string imm_val = to_bin(imm, 12);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rd_val = to_bin(rd, 5);
            std::string funct3_val = info.funct3;
            std::string opcode_val = info.opcode;


            binaryInstruction = imm_val + rs1_val + funct3_val + rd_val + opcode_val;
            qDebug() << "Assembled I-type (debug): imm_val=" << QString::fromStdString(imm_val)
                     << " rs1_val=" << QString::fromStdString(rs1_val)
                     << " funct3_val=" << QString::fromStdString(funct3_val)
                     << " rd_val=" << QString::fromStdString(rd_val)
                     << " opcode_val=" << QString::fromStdString(opcode_val)
                     << " result=" << QString::fromStdString(binaryInstruction);
        }
        else if (inst == "sh" || inst == "sw") {
            if (tokens.size() != 3) {
                qDebug() << "Invalid token count for S-type:" << tokens.size() << tokens;
                return binaryInstruction;
            }
            int rs2 = regMap.at(tokens[1]);
            std::string imm_rs1 = tokens[2];
            int imm, rs1;


            size_t pos = imm_rs1.find('(');
            if (pos == std::string::npos || pos == 0 || imm_rs1.back() != ')') {
                qDebug() << "Invalid imm(rs1) format:" << QString::fromStdString(imm_rs1);
                return binaryInstruction;
            }
            std::string imm_str = imm_rs1.substr(0, pos);
            imm = std::stoi(imm_str);
            std::string rs1_str = imm_rs1.substr(pos + 1, imm_rs1.length() - pos - 2);
            qDebug() << "Extracted: imm_str=" << QString::fromStdString(imm_str)
                     << " imm=" << imm
                     << " rs1_str=" << QString::fromStdString(rs1_str);
            if (regMap.find(rs1_str) == regMap.end()) {
                qDebug() << "Invalid register:" << QString::fromStdString(rs1_str);
                return binaryInstruction;
            }
            rs1 = regMap.at(rs1_str);

            std::string imm_val = to_bin(imm, 12);
            std::string immHi = imm_val.substr(0, 7);
            std::string immLo = imm_val.substr(5, 5);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rs2_val = to_bin(rs2, 5);
            std::string funct3_val = info.funct3;
            std::string opcode_val = info.opcode;


            binaryInstruction = immHi + rs2_val + rs1_val + funct3_val + immLo + opcode_val;
            qDebug() << "Assembled S-type (debug): immHi=" << QString::fromStdString(immHi)
                     << " rs2_val=" << QString::fromStdString(rs2_val)
                     << " rs1_val=" << QString::fromStdString(rs1_val)
                     << " funct3_val=" << QString::fromStdString(funct3_val)
                     << " immLo=" << QString::fromStdString(immLo)
                     << " opcode_val=" << QString::fromStdString(opcode_val)
                     << " result=" << QString::fromStdString(binaryInstruction);
        }
        else if (inst == "beq" || inst == "bne" || inst == "blt" || inst == "bge" || inst == "bltu" || inst == "bgeu") {
            if (tokens.size() != 4) return binaryInstruction;
            int rs1 = regMap.at(tokens[1]);
            int rs2 = regMap.at(tokens[2]);
            int imm = std::stoi(tokens[3]);

            std::string imm_val = to_bin(imm, 12);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rs2_val = to_bin(rs2, 5);
            std::string imm12 = imm_val.substr(0, 1);
            std::string imm105 = imm_val.substr(1, 6);
            std::string imm41 = imm_val.substr(7, 4);
            std::string imm11 = imm_val.substr(11, 1);

            binaryInstruction = imm12 + imm105 + rs2_val + rs1_val + info.funct3 + imm41 + imm11 + info.opcode;
        }
        else if (inst == "jal") {
            if (tokens.size() != 3) return binaryInstruction;
            int rd = regMap.at(tokens[1]);
            int imm = std::stoi(tokens[2]);

            std::string imm_val = to_bin(imm, 20);
            std::string rd_val = to_bin(rd, 5);
            std::string imm20 = imm_val.substr(0, 1);
            std::string imm101 = imm_val.substr(1, 10);
            std::string imm11 = imm_val.substr(11, 1);
            std::string imm1912 = imm_val.substr(12, 8);

            binaryInstruction = imm20 + imm101 + imm11 + imm1912 + rd_val + info.opcode;
        }
        else if (inst == "jalr" || inst == "lui" || inst == "auipc") {
            if (tokens.size() != 3) return binaryInstruction;
            int rd = regMap.at(tokens[1]);
            int imm = std::stoi(tokens[2]);

            std::string imm_val = to_bin(imm, 20);
            std::string rd_val = to_bin(rd, 5);

            binaryInstruction = imm_val + rd_val + info.opcode;
        }
    }
    catch (const std::exception& e) {
        qDebug() << "Exception:" << e.what();
        return binaryInstruction;
    }


    if (binaryInstruction.length() != 32) {
        if (binaryInstruction.length() > 32) {
            binaryInstruction = binaryInstruction.substr(binaryInstruction.length() - 32, 32);
        } else if (binaryInstruction.length() < 32) {
            binaryInstruction = std::string(32 - binaryInstruction.length(), '0') + binaryInstruction;
        }
    }

    return binaryInstruction;
}
