#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
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
    bool isCompiling = false; // Prevent multiple compilations

    struct InstrInfo {
        std::string funct7;  // 7 bits
        std::string rs2;     // 5 bits
        std::string rs1;     // 5 bits
        std::string funct3;  // 3 bits
        std::string rd;      // 5 bits
        std::string opcode;  // 7 bits
    };

    std::unordered_map<std::string, InstrInfo> instrMap;
    std::unordered_map<std::string, int> regMap;
    std::unordered_map<std::string, int> symbolTable;
    int registers[32] = {0};

    bool isNumeric(const std::string& str);
    std::string to_bin(int val, int bits);
    std::vector<std::string> tokenize(const std::string& line);
    std::string assemble(const std::string& line, int currentPC);
    void parseLabels();
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
        {"addi", {"0000000", "00000", "00000", "000", "00000", "0010011"}},
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
        {"jalr", {"0000000", "00000", "00000", "000", "00000", "1100111"}},
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

    disconnect(ui->compile_btn, &QPushButton::clicked, this, &MainWindow::on_compile_btn_clicked);
    connect(ui->compile_btn, &QPushButton::clicked, this, &MainWindow::on_compile_btn_clicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

bool MainWindow::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    if (str.find("0x") == 0 || str.find("0X") == 0) {
        try {
            size_t pos;
            std::stoul(str, &pos, 16);
            return pos == str.length();
        } catch (...) {
            return false;
        }
    }
    if (str.find("0b") == 0 || str.find("0B") == 0) {
        try {
            size_t pos;
            std::stoi(str.substr(2), &pos, 2);
            return pos == str.length() - 2;
        } catch (...) {
            return false;
        }
    }
    try {
        size_t pos;
        std::stoi(str, &pos);
        return pos == str.length();
    } catch (...) {
        return false;
    }
}

void MainWindow::parseLabels() {
    symbolTable.clear();
    QString fullText = ui->code_field->toPlainText();
    QStringList lines = fullText.split("\n", Qt::SkipEmptyParts);
    int pc = 0;

    for (int i = 0; i < lines.size(); ++i) {
        std::string line = lines[i].toStdString();
        std::string trimmed_line = line;
        trimmed_line.erase(std::remove_if(trimmed_line.begin(), trimmed_line.end(), ::isspace), trimmed_line.end());
        if (trimmed_line.empty()) continue;

        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string label = trimmed_line.substr(0, colon_pos);
            if (!label.empty()) {
                symbolTable[label] = pc;
                qDebug() << "Label found:" << QString::fromStdString(label) << "at address" << pc;
            }
            if (colon_pos + 1 < line.length()) {
                std::string remaining = line.substr(colon_pos + 1);
                remaining.erase(0, remaining.find_first_not_of(" \t"));
                std::vector<std::string> tokens = tokenize(remaining);
                if (!tokens.empty() && instrMap.find(tokens[0]) != instrMap.end()) {
                    pc += 4;
                }
            }
            continue;
        }

        std::vector<std::string> tokens = tokenize(line);
        if (!tokens.empty() && instrMap.find(tokens[0]) != instrMap.end()) {
            pc += 4;
        } else {
            qDebug() << "Invalid instruction or empty line at" << i << ":" << QString::fromStdString(line);
        }
    }
}

void MainWindow::on_compile_btn_clicked() {
    if (isCompiling) {
        qDebug() << "Compilation is in progress.";
        return;
    }
    isCompiling = true;

    QString fullText = ui->code_field->toPlainText();
    QStringList lines = fullText.split("\n", Qt::SkipEmptyParts);
    QString output;

    parseLabels();

    int pc = 0;
    for (const QString& line : lines) {
        std::string stdLine = line.toStdString();
        std::vector<std::string> tokens = tokenize(stdLine);
        std::string binaryInstruction = assemble(stdLine, pc);
        if (!binaryInstruction.empty()) {
            output += QString::fromStdString(binaryInstruction) + "\n";
            qDebug() << "Output for line:" << line << QString::fromStdString(binaryInstruction);
        } else {
            qDebug() << "No output for line:" << line;
        }
        if (!tokens.empty() && instrMap.find(tokens[0]) != instrMap.end()) {
            pc += 4;
        }
    }

    if (ui->output_field) {
        ui->output_field->setPlainText(output);
    } else {
        QMessageBox::information(this, "Assembly Output", output);
    }

    isCompiling = false;
}

std::string MainWindow::to_bin(int val, int bits) {
    std::string res(bits, '0');
    unsigned long long uval;
    if (val < 0) {
        uval = (1ULL << bits) + val;
    } else {
        uval = static_cast<unsigned long long>(val);
    }
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

    std::string trimmed_line = line;
    trimmed_line.erase(0, trimmed_line.find_first_not_of(" \t"));
    trimmed_line.erase(trimmed_line.find_last_not_of(" \t") + 1);
    if (trimmed_line.empty()) {
        return tokens;
    }

    size_t colon_pos = trimmed_line.find(':');
    if (colon_pos != std::string::npos) {
        std::string label = trimmed_line.substr(0, colon_pos);
        tokens.push_back(label + ":");
        if (colon_pos + 1 < trimmed_line.length()) {
            trimmed_line = line.substr(line.find(':') + 1);
            trimmed_line.erase(0, trimmed_line.find_first_not_of(" \t"));
        } else {
            qDebug() << "Tokens for line:" << QString::fromStdString(line) << tokens.size() << tokens;
            return tokens;
        }
    }

    token.clear();
    bool instruction_found = false;
    size_t i = 0;
    for (const auto& instr : instrMap) {
        std::string inst = instr.first;
        if (trimmed_line.length() >= inst.length() &&
            trimmed_line.substr(0, inst.length()) == inst &&
            (trimmed_line.length() == inst.length() ||
             trimmed_line[inst.length()] == ' ' ||
             trimmed_line[inst.length()] == ',')) {
            tokens.push_back(inst);
            i = inst.length();
            instruction_found = true;
            break;
        }
    }

    if (!instruction_found) {
        if (!trimmed_line.empty()) {
            tokens.push_back(trimmed_line);
        }
        qDebug() << "Tokens for line:" << QString::fromStdString(line) << tokens.size() << tokens;
        return tokens;
    }

    token.clear();
    bool expect_operand = true;
    for (; i < trimmed_line.length(); ++i) {
        char c = trimmed_line[i];

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
                expect_operand = false;
            }
            continue;
        }

        if (c == ',' && !inParentheses) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
                expect_operand = true;
            }
            continue;
        }

        if (c == ' ' && !inParentheses) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
                expect_operand = true;
            }
            continue;
        }

        token += c;
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }

    qDebug() << "Tokens for line:" << QString::fromStdString(line) << tokens.size() << tokens;
    return tokens;
}

std::string MainWindow::assemble(const std::string& line, int currentPC) {
    std::string binaryInstruction;
    std::vector<std::string> tokens = tokenize(line);
    if (tokens.empty()) {
        qDebug() << "Empty tokens for line:" << QString::fromStdString(line);
        return binaryInstruction;
    }

    if (tokens[0].back() == ':') {
        return binaryInstruction;
    }

    std::string inst = tokens[0];
    if (instrMap.find(inst) == instrMap.end()) {
        qDebug() << "Instruction not found:" << QString::fromStdString(inst);
        return binaryInstruction;
    }

    InstrInfo info = instrMap[inst];

    try {
        // Helper function to parse immediate values (Decimal, Hex, Binary)
        auto parseImmediate = [](const std::string& imm_str) -> int {
            try {
                if (imm_str.find("0b") == 0 || imm_str.find("0B") == 0) {
                    // Binary format
                    std::string binary = imm_str.substr(2);
                    return std::stoi(binary, nullptr, 2);
                } else if (imm_str.find("0x") == 0 || imm_str.find("0X") == 0) {
                    // Hexadecimal format
                    return std::stoi(imm_str, nullptr, 16);
                } else {
                    // Decimal format
                    return std::stoi(imm_str);
                }
            } catch (const std::exception& e) {
                throw std::runtime_error("Invalid immediate value: " + imm_str);
            }
        };

        if (inst == "add" || inst == "sub" || inst == "xor" || inst == "or" || inst == "and" ||
            inst == "sll" || inst == "srl" || inst == "sra" || inst == "slt" || inst == "sltu" ||
            inst == "mul" || inst == "mulh" || inst == "div" || inst == "rem") {
            if (tokens.size() != 4) {
                throw std::runtime_error("Invalid number of tokens for R-type: " + std::to_string(tokens.size()));
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
            if (tokens.size() != 3 && tokens.size() != 4) {
                throw std::runtime_error("Invalid number of tokens for I-type: " + std::to_string(tokens.size()));
            }
            int rd = regMap.at(tokens[1]);
            std::string rs1_str, imm_str;
            int rs1, imm;

            if (inst == "addi") {
                if (tokens.size() != 4) {
                    throw std::runtime_error("Invalid number of tokens for addi: " + std::to_string(tokens.size()));
                }
                rs1_str = tokens[2];
                imm_str = tokens[3];
                imm = parseImmediate(imm_str);
                if (regMap.find(rs1_str) == regMap.end()) {
                    throw std::runtime_error("Invalid register: " + rs1_str);
                }
                rs1 = regMap.at(rs1_str);
            } else {
                if (tokens.size() != 3) {
                    throw std::runtime_error("Invalid number of tokens for load/jalr: " + std::to_string(tokens.size()));
                }
                std::string imm_rs1 = tokens[2];
                size_t pos = imm_rs1.find('(');
                if (pos == std::string::npos || pos == 0 || imm_rs1.back() != ')') {
                    throw std::runtime_error("Invalid imm(rs1) format: " + imm_rs1);
                }
                imm_str = imm_rs1.substr(0, pos);
                imm = parseImmediate(imm_str);
                rs1_str = imm_rs1.substr(pos + 1, imm_rs1.length() - pos - 2);
                if (regMap.find(rs1_str) == regMap.end()) {
                    throw std::runtime_error("Invalid register: " + rs1_str);
                }
                rs1 = regMap.at(rs1_str);
            }

            // Validate immediate value range for I-type (12-bit signed)
            if (imm < -2048 || imm > 2047) {
                throw std::runtime_error("Immediate value out of range for I-type: " + std::to_string(imm));
            }

            std::string imm_val = to_bin(imm, 12);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rd_val = to_bin(rd, 5);

            binaryInstruction = imm_val + rs1_val + info.funct3 + rd_val + info.opcode;
        }
        else if (inst == "sh" || inst == "sw") {
            if (tokens.size() != 3) {
                throw std::runtime_error("Invalid number of tokens for S-type: " + std::to_string(tokens.size()));
            }
            int rs2 = regMap.at(tokens[1]);
            std::string imm_rs1 = tokens[2];
            int imm, rs1;

            size_t pos = imm_rs1.find('(');
            if (pos == std::string::npos || pos == 0 || imm_rs1.back() != ')') {
                throw std::runtime_error("Invalid imm(rs1) format: " + imm_rs1);
            }
            std::string imm_str = imm_rs1.substr(0, pos);
            imm = parseImmediate(imm_str);
            std::string rs1_str = imm_rs1.substr(pos + 1, imm_rs1.length() - pos - 2);
            if (regMap.find(rs1_str) == regMap.end()) {
                throw std::runtime_error("Invalid register: " + rs1_str);
            }
            rs1 = regMap.at(rs1_str);

            // Validate immediate value range for S-type (12-bit signed)
            if (imm < -2048 || imm > 2047) {
                throw std::runtime_error("Immediate value out of range for S-type: " + std::to_string(imm));
            }

            std::string imm_val = to_bin(imm, 12);
            std::string immHi = imm_val.substr(0, 7);
            std::string immLo = imm_val.substr(7, 5);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rs2_val = to_bin(rs2, 5);

            binaryInstruction = immHi + rs2_val + rs1_val + info.funct3 + immLo + info.opcode;
        }
        else if (inst == "beq" || inst == "bne" || inst == "blt" || inst == "bge" || inst == "bltu" || inst == "bgeu") {
            if (tokens.size() != 4) {
                throw std::runtime_error("Invalid number of tokens for B-type: " + std::to_string(tokens.size()));
            }
            int rs2 = regMap.at(tokens[1]);
            int rs1 = regMap.at(tokens[2]);
            std::string target = tokens[3];
            int imm;

            if (isNumeric(target)) {
                imm = parseImmediate(target);
            } else {
                if (symbolTable.find(target) == symbolTable.end()) {
                    throw std::runtime_error("Label not found: " + target);
                }
                int targetPC = symbolTable.at(target);
                imm = targetPC - currentPC;
            }

            // Validate immediate value range for B-type (13-bit signed, must be multiple of 2)
            if (imm % 2 != 0) {
                throw std::runtime_error("B-type immediate value must be a multiple of 2: " + std::to_string(imm));
            }
            if (imm < -4096 || imm > 4095) {
                throw std::runtime_error("Immediate value out of range for B-type: " + std::to_string(imm));
            }

            std::string imm_val = to_bin(imm, 13);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rs2_val = to_bin(rs2, 5);
            std::string imm12 = imm_val.substr(0, 1);
            std::string imm105 = imm_val.substr(1, 6);
            std::string imm41 = imm_val.substr(8, 4);
            std::string imm11 = imm_val.substr(12, 1);

            binaryInstruction = imm12 + imm105 + rs2_val + rs1_val + info.funct3 + imm41 + imm11 + info.opcode;
        }
        else if (inst == "jal") {
            if (tokens.size() != 3) {
                throw std::runtime_error("Invalid number of tokens for J-type: " + std::to_string(tokens.size()));
            }
            int rd = regMap.at(tokens[1]);
            std::string target = tokens[2];
            int imm;

            if (isNumeric(target)) {
                imm = parseImmediate(target);
            } else {
                if (symbolTable.find(target) == symbolTable.end()) {
                    throw std::runtime_error("Label not found: " + target);
                }
                int targetPC = symbolTable.at(target);
                imm = targetPC - currentPC;
            }

            // Validate immediate value range for J-type (20-bit signed, must be multiple of 2)
            if (imm % 2 != 0) {
                throw std::runtime_error("JAL immediate value must be a multiple of 2: " + std::to_string(imm));
            }
            if (imm < -1048576 || imm > 1048575) {
                throw std::runtime_error("Immediate value out of range for J-type: " + std::to_string(imm));
            }

            std::string imm_val = to_bin(imm, 20);
            std::string rd_val = to_bin(rd, 5);
            std::string imm20 = imm_val.substr(0, 1);
            std::string imm10_1 = imm_val.substr(1, 10);
            std::string imm11 = imm_val.substr(11, 1);
            std::string imm19_12 = imm_val.substr(12, 8);

            binaryInstruction = imm20 + imm10_1 + imm11 + imm19_12 + rd_val + info.opcode;
        }
        else if (inst == "lui" || inst == "auipc") {
            if (tokens.size() != 3) {
                throw std::runtime_error("Invalid number of tokens for U-type: " + std::to_string(tokens.size()));
            }
            int rd = regMap.at(tokens[1]);
            std::string imm_str = tokens[2];
            int imm = parseImmediate(imm_str);

            // Validate immediate value range for U-type (20-bit)
            if (imm < 0 || imm > 1048575) {
                throw std::runtime_error("Immediate value out of range for U-type: " + std::to_string(imm));
            }

            std::string imm_val = to_bin(imm, 20);
            std::string rd_val = to_bin(rd, 5);

            binaryInstruction = imm_val + rd_val + info.opcode;
        }
    }
    catch (const std::exception& e) {
        qDebug() << "Error in assembling:" << e.what();
        QMessageBox::warning(this, "Assembly Error", QString("Error in line: %1\n%2").arg(QString::fromStdString(line), QString::fromStdString(e.what())));
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
