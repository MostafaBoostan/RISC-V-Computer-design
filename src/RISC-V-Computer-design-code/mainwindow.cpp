#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
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
    std::vector<uint8_t> memory; // Simulated memory
    int currentAddress = 0; // Current memory address for directives

    bool isNumeric(const std::string& str);
    std::string to_bin(int val, int bits);
    std::vector<std::string> tokenize(const std::string& line);
    std::string assemble(const std::string& line, int& currentPC);
    void parseLabels();
    void storeInMemory(int value, int size, int& address);
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

    // Initialize memory (e.g., 1MB)
    memory.resize(1024 * 1024, 0);

    disconnect(ui->compile_btn, &QPushButton::clicked, this, &MainWindow::on_compile_btn_clicked);
    connect(ui->compile_btn, &QPushButton::clicked, this, &MainWindow::on_compile_btn_clicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

bool MainWindow::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    // Check for character literal (e.g., 'A')
    if (str.length() == 3 && str[0] == '\'' && str[2] == '\'') {
        return true;
    }
    // Check for hexadecimal
    if (str.find("0x") == 0 || str.find("0X") == 0) {
        std::string hex = str.substr(2);
        return !hex.empty() && std::all_of(hex.begin(), hex.end(), ::isxdigit);
    }
    // Check for binary
    if (str.find("0b") == 0 || str.find("0B") == 0) {
        std::string bin = str.substr(2);
        return !bin.empty() && std::all_of(bin.begin(), bin.end(), [](char c) { return c == '0' || c == '1'; });
    }
    // Check for decimal
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
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
                if (!tokens.empty()) {
                    if (instrMap.find(tokens[0]) != instrMap.end()) {
                        pc += 4;
                    } else if (tokens[0] == ".word") {
                        pc += 4;
                    } else if (tokens[0] == ".half") {
                        pc += 2;
                    } else if (tokens[0] == ".byte") {
                        pc += 1;
                    } else if (tokens[0] == ".org" || tokens[0] == ".align") {
                        // Handled in assemble
                    }
                }
            }
            continue;
        }

        std::vector<std::string> tokens = tokenize(line);
        if (!tokens.empty()) {
            if (instrMap.find(tokens[0]) != instrMap.end()) {
                pc += 4;
            } else if (tokens[0] == ".word") {
                pc += 4;
            } else if (tokens[0] == ".half") {
                pc += 2;
            } else if (tokens[0] == ".byte") {
                pc += 1;
            } else if (tokens[0] == ".org" || tokens[0] == ".align") {
                // Handled in assemble
            } else {
                qDebug() << "Invalid instruction or directive at" << i << ":" << QString::fromStdString(line);
            }
        }
    }
}

void MainWindow::storeInMemory(int value, int size, int& address) {
    if (address + size > memory.size()) {
        memory.resize(address + size, 0);
    }
    // Store in little-endian format
    for (int i = 0; i < size; ++i) {
        memory[address + i] = (value >> (i * 8)) & 0xFF;
    }
    address += size;
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
    currentAddress = 0;
    memory.clear();
    memory.resize(1024 * 1024, 0);

    for (const QString& line : lines) {
        std::string stdLine = line.toStdString();
        std::vector<std::string> tokens = tokenize(stdLine);
        std::string binaryInstruction;
        try {
            binaryInstruction = assemble(stdLine, pc);
            if (!binaryInstruction.empty()) {
                output += QString::fromStdString(binaryInstruction) + "\n";
                qDebug() << "Output for line:" << line << QString::fromStdString(binaryInstruction);
            } else {
                qDebug() << "No output for line:" << line;
            }
        } catch (const std::exception& e) {
            output += QString("Error in line: %1\n%2\n").arg(line, QString::fromStdString(e.what()));
            qDebug() << "Error in line:" << line << QString::fromStdString(e.what());
        }
        if (!tokens.empty()) {
            if (instrMap.find(tokens[0]) != instrMap.end()) {
                pc += 4;
            } else if (tokens[0] == ".word") {
                pc += 4;
            } else if (tokens[0] == ".half") {
                pc += 2;
            } else if (tokens[0] == ".byte") {
                pc += 1;
            }
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

    // Trim leading and trailing whitespace
    std::string trimmed_line = line;
    auto start = std::find_if(trimmed_line.begin(), trimmed_line.end(), [](unsigned char c) { return !std::isspace(c); });
    trimmed_line = std::string(start, trimmed_line.end());
    auto end = std::find_if(trimmed_line.rbegin(), trimmed_line.rend(), [](unsigned char c) { return !std::isspace(c); }).base();
    trimmed_line = std::string(trimmed_line.begin(), end);

    if (trimmed_line.empty()) {
        return tokens;
    }

    // Handle labels
    size_t colon_pos = trimmed_line.find(':');
    if (colon_pos != std::string::npos) {
        std::string label = trimmed_line.substr(0, colon_pos);
        label.erase(std::remove_if(label.begin(), label.end(), ::isspace), label.end());
        if (!label.empty()) {
            tokens.push_back(label + ":");
        }
        if (colon_pos + 1 < trimmed_line.length()) {
            trimmed_line = trimmed_line.substr(colon_pos + 1);
            start = std::find_if(trimmed_line.begin(), trimmed_line.end(), [](unsigned char c) { return !std::isspace(c); });
            trimmed_line = std::string(start, trimmed_line.end());
        } else {
            qDebug() << "Tokens for line:" << QString::fromStdString(line) << QString::number(tokens.size()) << (tokens.empty() ? "" : QString::fromStdString(tokens.back()));
            return tokens;
        }
    }

    // Split into tokens
    token.clear();
    bool instruction_found = false;
    size_t i = 0;

    // Check for instructions
    for (const auto& instr : instrMap) {
        std::string inst = instr.first;
        if (trimmed_line.length() >= inst.length() &&
            trimmed_line.substr(0, inst.length()) == inst &&
            (trimmed_line.length() == inst.length() ||
             std::isspace(trimmed_line[inst.length()]) ||
             trimmed_line[inst.length()] == ',')) {
            tokens.push_back(inst);
            i = inst.length();
            instruction_found = true;
            break;
        }
    }

    // Check for directives if no instruction found
    if (!instruction_found) {
        std::vector<std::string> directives = {".org", ".word", ".half", ".byte", ".align"};
        for (const auto& directive : directives) {
            if (trimmed_line.length() >= directive.length() &&
                trimmed_line.substr(0, directive.length()) == directive &&
                (trimmed_line.length() == directive.length() ||
                 std::isspace(trimmed_line[directive.length()]) ||
                 trimmed_line[directive.length()] == ',')) {
                tokens.push_back(directive);
                i = directive.length();
                instruction_found = true;
                break;
            }
        }
    }

    // If neither instruction nor directive, treat as invalid
    if (!instruction_found && !trimmed_line.empty()) {
        tokens.push_back(trimmed_line);
        qDebug() << "Tokens for line:" << QString::fromStdString(line) << QString::number(tokens.size()) << QString::fromStdString(tokens.back());
        return tokens;
    }

    // Process remaining tokens
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

        if ((c == ',' || std::isspace(c)) && !inParentheses) {
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

    // Remove empty tokens
    tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [](const std::string& t) { return t.empty(); }), tokens.end());

    qDebug() << "Tokens for line:" << QString::fromStdString(line) << QString::number(tokens.size()) << (tokens.empty() ? "" : QString::fromStdString(tokens[0]));
    return tokens;
}

std::string MainWindow::assemble(const std::string& line, int& currentPC) {
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
    if (instrMap.find(inst) == instrMap.end() && inst != ".org" && inst != ".word" &&
        inst != ".half" && inst != ".byte" && inst != ".align") {
        throw std::runtime_error("Instruction or directive not found: " + inst);
    }

    // Helper function to parse immediate values
    auto parseImmediate = [inst](const std::string& imm_str) -> unsigned long {
        try {
            // Check for character literal (e.g., 'A'), only allowed for .byte
            if (imm_str.length() == 3 && imm_str[0] == '\'' && imm_str[2] == '\'') {
                if (inst != ".byte") {
                    throw std::runtime_error("Character literals like '" + imm_str + "' are only allowed with .byte directive");
                }
                char c = imm_str[1];
                return static_cast<unsigned long>(c);
            }
            // Check for binary
            if (imm_str.find("0b") == 0 || imm_str.find("0B") == 0) {
                std::string binary = imm_str.substr(2);
                binary.erase(std::remove_if(binary.begin(), binary.end(), ::isspace), binary.end());
                if (binary.empty() || !std::all_of(binary.begin(), binary.end(), [](char c) { return c == '0' || c == '1'; })) {
                    throw std::runtime_error("Invalid binary value: " + imm_str);
                }
                return std::stoul(binary, nullptr, 2);
            }
            // Check for hexadecimal
            if (imm_str.find("0x") == 0 || imm_str.find("0X") == 0) {
                std::string hex = imm_str.substr(2);
                hex.erase(std::remove_if(hex.begin(), hex.end(), ::isspace), hex.end());
                if (hex.empty() || !std::all_of(hex.begin(), hex.end(), ::isxdigit)) {
                    throw std::runtime_error("Invalid hexadecimal value: " + imm_str);
                }
                qDebug() << "Parsing hex:" << QString::fromStdString(hex);
                return std::stoul(hex, nullptr, 16);
            }
            // Decimal
            std::string trimmed = imm_str;
            trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), ::isspace), trimmed.end());
            if (trimmed.empty() || !std::all_of(trimmed.begin(), trimmed.end(), ::isdigit)) {
                throw std::runtime_error("Invalid decimal value: " + imm_str);
            }
            return std::stoul(trimmed, nullptr, 10);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid immediate value: " + imm_str + " (" + e.what() + ")");
        }
    };

    try {
        // Handle directives
        if (inst == ".org") {
            if (tokens.size() != 2) {
                throw std::runtime_error(".org expects 1 argument, got " + std::to_string(tokens.size() - 1));
            }
            unsigned long address = parseImmediate(tokens[1]);
            if (address > 0xFFFFFFFF) {
                throw std::runtime_error("Invalid .org address: " + tokens[1]);
            }
            currentPC = static_cast<int>(address);
            currentAddress = static_cast<int>(address);
            return binaryInstruction;
        }
        else if (inst == ".word") {
            if (tokens.size() != 2) {
                throw std::runtime_error(".word expects 1 argument, got " + std::to_string(tokens.size() - 1));
            }
            unsigned long value = parseImmediate(tokens[1]);
            storeInMemory(static_cast<int>(value), 4, currentAddress);
            binaryInstruction = to_bin(static_cast<int>(value), 32);
            currentPC += 4;
            return binaryInstruction;
        }
        else if (inst == ".half") {
            if (tokens.size() != 2) {
                throw std::runtime_error(".half expects 1 argument, got " + std::to_string(tokens.size() - 1));
            }
            unsigned long value = parseImmediate(tokens[1]);
            if (value > 65535) {
                throw std::runtime_error("Value out of range for .half: " + std::to_string(value));
            }
            storeInMemory(static_cast<int>(value), 2, currentAddress);
            binaryInstruction = to_bin(static_cast<int>(value), 32);
            currentPC += 2;
            return binaryInstruction;
        }
        else if (inst == ".byte") {
            if (tokens.size() != 2) {
                throw std::runtime_error(".byte expects 1 argument, got " + std::to_string(tokens.size() - 1));
            }
            unsigned long value = parseImmediate(tokens[1]);
            if (value > 255) {
                throw std::runtime_error("Value out of range for .byte: " + std::to_string(value));
            }
            storeInMemory(static_cast<int>(value), 1, currentAddress);
            binaryInstruction = to_bin(static_cast<int>(value), 32);
            currentPC += 1;
            return binaryInstruction;
        }
        else if (inst == ".align") {
            if (tokens.size() != 2) {
                throw std::runtime_error(".align expects 1 argument, got " + std::to_string(tokens.size() - 1));
            }
            unsigned long n = parseImmediate(tokens[1]);
            if (n > 31) {
                throw std::runtime_error("Invalid alignment value: " + tokens[1]);
            }
            int alignment = 1 << n;
            int newAddress = ((currentPC + alignment - 1) / alignment) * alignment;
            currentPC = newAddress;
            currentAddress = newAddress;
            return binaryInstruction;
        }

        // Handle regular instructions
        InstrInfo info = instrMap[inst];

        if (inst == "add" || inst == "sub" || inst == "xor" || inst == "or" || inst == "and" ||
            inst == "sll" || inst == "srl" || inst == "sra" || inst == "slt" || inst == "sltu" ||
            inst == "mul" || inst == "mulh" || inst == "div" || inst == "rem") {
            if (tokens.size() != 4) {
                throw std::runtime_error(inst + " expects 3 arguments (rd, rs1, rs2), got " + std::to_string(tokens.size() - 1));
            }
            int rd = regMap.at(tokens[1]);
            int rs1 = regMap.at(tokens[2]);
            int rs2 = regMap.at(tokens[3]);

            std::string rd_val = to_bin(rd, 5);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rs2_val = to_bin(rs2, 5);

            binaryInstruction = info.funct7 + rs2_val + rs1_val + info.funct3 + rd_val + info.opcode;
            currentPC += 4;
        }
        else if (inst == "addi") {
            if (tokens.size() != 4) {
                throw std::runtime_error("addi expects 3 arguments (rd, rs1, imm), got " + std::to_string(tokens.size() - 1));
            }
            int rd = regMap.at(tokens[1]);
            int rs1 = regMap.at(tokens[2]);
            unsigned long imm = parseImmediate(tokens[3]);

            if (imm > 2047 && imm <= 0xFFFFFFFF) imm = -(0x1000 - (imm & 0xFFF));
            if (static_cast<int>(imm) < -2048 || static_cast<int>(imm) > 2047) {
                throw std::runtime_error("Immediate value out of range for addi: " + std::to_string(imm));
            }

            std::string imm_val = to_bin(static_cast<int>(imm), 12);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rd_val = to_bin(rd, 5);

            binaryInstruction = imm_val + rs1_val + info.funct3 + rd_val + info.opcode;
            currentPC += 4;
        }
        else if (inst == "lh" || inst == "lw" || inst == "jalr") {
            if (tokens.size() != 3) {
                throw std::runtime_error(inst + " expects 2 arguments (rd, imm(rs1)), got " + std::to_string(tokens.size() - 1));
            }
            int rd = regMap.at(tokens[1]);
            std::string imm_rs1 = tokens[2];
            size_t pos = imm_rs1.find('(');
            if (pos == std::string::npos || pos == 0 || imm_rs1.back() != ')') {
                throw std::runtime_error("Invalid imm(rs1) format: " + imm_rs1);
            }
            std::string imm_str = imm_rs1.substr(0, pos);
            unsigned long imm = parseImmediate(imm_str);
            std::string rs1_str = imm_rs1.substr(pos + 1, imm_rs1.length() - pos - 2);
            if (regMap.find(rs1_str) == regMap.end()) {
                throw std::runtime_error("Invalid register: " + rs1_str);
            }
            int rs1 = regMap.at(rs1_str);

            if (static_cast<int>(imm) < -2048 || static_cast<int>(imm) > 2047) {
                throw std::runtime_error("Immediate value out of range for " + inst + ": " + std::to_string(imm));
            }

            std::string imm_val = to_bin(static_cast<int>(imm), 12);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rd_val = to_bin(rd, 5);

            binaryInstruction = imm_val + rs1_val + info.funct3 + rd_val + info.opcode;
            currentPC += 4;
        }
        else if (inst == "sh" || inst == "sw") {
            if (tokens.size() != 3) {
                throw std::runtime_error(inst + " expects 2 arguments (rs2, imm(rs1)), got " + std::to_string(tokens.size() - 1));
            }
            int rs2 = regMap.at(tokens[1]);
            std::string imm_rs1 = tokens[2];
            size_t pos = imm_rs1.find('(');
            if (pos == std::string::npos || pos == 0 || imm_rs1.back() != ')') {
                throw std::runtime_error("Invalid imm(rs1) format: " + imm_rs1);
            }
            std::string imm_str = imm_rs1.substr(0, pos);
            unsigned long imm = parseImmediate(imm_str);
            std::string rs1_str = imm_rs1.substr(pos + 1, imm_rs1.length() - pos - 2);
            if (regMap.find(rs1_str) == regMap.end()) {
                throw std::runtime_error("Invalid register: " + rs1_str);
            }
            int rs1 = regMap.at(rs1_str);

            if (static_cast<int>(imm) < -2048 || static_cast<int>(imm) > 2047) {
                throw std::runtime_error("Immediate value out of range for " + inst + ": " + std::to_string(imm));
            }

            std::string imm_val = to_bin(static_cast<int>(imm), 12);
            std::string immHi = imm_val.substr(0, 7);
            std::string immLo = imm_val.substr(7, 5);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rs2_val = to_bin(rs2, 5);

            binaryInstruction = immHi + rs2_val + rs1_val + info.funct3 + immLo + info.opcode;
            currentPC += 4;
        }
        else if (inst == "beq" || inst == "bne" || inst == "blt" || inst == "bge" || inst == "bltu" || inst == "bgeu") {
            if (tokens.size() != 4) {
                throw std::runtime_error(inst + " expects 3 arguments (rs1, rs2, label/imm), got " + std::to_string(tokens.size() - 1));
            }
            int rs1 = regMap.at(tokens[1]);
            int rs2 = regMap.at(tokens[2]);
            std::string target = tokens[3];
            target.erase(std::remove_if(target.begin(), target.end(), ::isspace), target.end());
            int imm;

            if (isNumeric(target)) {
                imm = static_cast<int>(parseImmediate(target));
            } else {
                if (symbolTable.find(target) == symbolTable.end()) {
                    throw std::runtime_error("Label not found: " + target);
                }
                int targetPC = symbolTable.at(target);
                imm = targetPC - currentPC;
                qDebug() << "beq: targetPC =" << targetPC << ", currentPC =" << currentPC << ", imm =" << imm;
            }

            if (imm % 2 != 0) {
                throw std::runtime_error("B-type immediate value must be a multiple of 2: " + std::to_string(imm));
            }
            if (imm < -4096 || imm > 4095) {
                throw std::runtime_error("Immediate value out of range for " + inst + ": " + std::to_string(imm));
            }

            std::string imm_val = to_bin(imm, 13);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rs2_val = to_bin(rs2, 5);
            std::string imm12 = imm_val.substr(0, 1);
            std::string imm105 = imm_val.substr(1, 6);
            std::string imm41 = imm_val.substr(8, 4);
            std::string imm11 = imm_val.substr(12, 1);

            binaryInstruction = imm12 + imm105 + rs2_val + rs1_val + info.funct3 + imm41 + imm11 + info.opcode;
            currentPC += 4;
        }
        else if (inst == "jal") {
            if (tokens.size() != 3) {
                throw std::runtime_error("jal expects 2 arguments (rd, label/imm), got " + std::to_string(tokens.size() - 1));
            }
            int rd = regMap.at(tokens[1]);
            std::string target = tokens[2];
            target.erase(std::remove_if(target.begin(), target.end(), ::isspace), target.end());
            int imm;

            if (isNumeric(target)) {
                imm = static_cast<int>(parseImmediate(target));
            } else {
                if (symbolTable.find(target) == symbolTable.end()) {
                    throw std::runtime_error("Label not found: " + target);
                }
                int targetPC = symbolTable.at(target);
                imm = targetPC - currentPC;
            }

            if (imm % 2 != 0) {
                throw std::runtime_error("JAL immediate value must be a multiple of 2: " + std::to_string(imm));
            }
            if (imm < -1048576 || imm > 1048575) {
                throw std::runtime_error("Immediate value out of range for jal: " + std::to_string(imm));
            }

            std::string imm_val = to_bin(imm, 20);
            std::string rd_val = to_bin(rd, 5);
            std::string imm20 = imm_val.substr(0, 1);
            std::string imm10_1 = imm_val.substr(1, 10);
            std::string imm11 = imm_val.substr(11, 1);
            std::string imm19_12 = imm_val.substr(12, 8);

            binaryInstruction = imm20 + imm10_1 + imm11 + imm19_12 + rd_val + info.opcode;
            currentPC += 4;
        }
        else if (inst == "lui" || inst == "auipc") {
            if (tokens.size() != 3) {
                throw std::runtime_error(inst + " expects 2 arguments (rd, imm), got " + std::to_string(tokens.size() - 1));
            }
            int rd = regMap.at(tokens[1]);
            unsigned long imm = parseImmediate(tokens[2]);

            if (imm > 1048575) {
                throw std::runtime_error("Immediate value out of range for " + inst + ": " + std::to_string(imm));
            }

            std::string imm_val = to_bin(static_cast<int>(imm), 20);
            std::string rd_val = to_bin(rd, 5);

            binaryInstruction = imm_val + rd_val + info.opcode;
            currentPC += 4;
        }
    }
    catch (const std::exception& e) {
        throw std::runtime_error(e.what());
    }

    if (binaryInstruction.length() != 32 && inst != ".word" && inst != ".half" && inst != ".byte") {
        if (binaryInstruction.length() > 32) {
            binaryInstruction = binaryInstruction.substr(binaryInstruction.length() - 32, 32);
        } else if (binaryInstruction.length() < 32) {
            binaryInstruction = std::string(32 - binaryInstruction.length(), '0') + binaryInstruction;
        }
    }

    return binaryInstruction;
}
