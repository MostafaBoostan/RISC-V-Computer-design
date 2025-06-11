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
#include <QStandardItem>
#include <QString>
#include <math.h>
#include "ui_mainwindow.h"
#include "mainwindow.h"

#define R "0110011"
#define I1 "0010011"
#define I2 "0000011"
#define I3 "1100111"
#define S "0100011"
#define B "1100011"
#define J "1101111"
#define U1 "0110111"
#define U2 "0010111"


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
        {"xori", {"0000000", "00000", "00000", "100", "00000", "0010011"}},
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
        {"rem",  {"0000001", "00000", "00000", "110", "00000", "0110011"}},
        {"nop",  {"0000000", "00000", "00000", "000", "00000", "0010011"}},
        {"li",   {"0000000", "00000", "00000", "000", "00000", "0010011"}},
        {"mv",   {"0000000", "00000", "00000", "000", "00000", "0010011"}},
        {"not",  {"0000000", "00000", "00000", "100", "00000", "0010011"}},
        {"neg",  {"0100000", "00000", "00000", "000", "00000", "0110011"}}
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

    memory.resize(1024 * 1024, 0);
    memoryArray.resize(1 << 18, 0);

    // Ensure single connection
    disconnect(ui->compile_btn, &QPushButton::clicked, this, &MainWindow::on_compile_btn_clicked);
    connect(ui->compile_btn, &QPushButton::clicked, this, &MainWindow::on_compile_btn_clicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

bool MainWindow::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    if (str.length() == 3 && str[0] == '\'' && str[2] == '\'') {
        return true;
    }
    if (str.find("0x") == 0 || str.find("0X") == 0) {
        std::string hex = str.substr(2);
        return !hex.empty() && std::all_of(hex.begin(), hex.end(), ::isxdigit);
    }
    if (str.find("0b") == 0 || str.find("0B") == 0) {
        std::string bin = str.substr(2);
        return !bin.empty() && std::all_of(bin.begin(), bin.end(), [](char c) { return c == '0' || c == '1'; });
    }
    std::string trimmed = str;
    trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), ::isspace), trimmed.end());
    if (trimmed.empty()) return false;
    size_t start = (trimmed[0] == '-') ? 1 : 0;
    return std::all_of(trimmed.begin() + start, trimmed.end(), ::isdigit);
}

int MainWindow::upper(long long imm) {
    int upper_bits = (imm >> 12) & 0xFFFFF;
    if (imm < 0 && (imm & 0xFFF) != 0) {
        upper_bits += 1;
    }
    return upper_bits;
}

int MainWindow::lower(long long imm) {
    int lower_bits = imm & 0xFFF;
    if (lower_bits & 0x800) {
        lower_bits |= 0xFFFFF000;
    }
    return lower_bits;
}

void MainWindow::parseLabels() {
    symbolTable.clear();
    QString fullText = ui->code_field->toPlainText();
    QStringList lines = fullText.split("\n", Qt::SkipEmptyParts);
    int pc = 0;

    auto parseImmediate = [](const std::string& imm_str) -> long long {
        try {
            if (imm_str.find("0x") == 0 || imm_str.find("0X") == 0) {
                std::string hex = imm_str.substr(2);
                hex.erase(std::remove_if(hex.begin(), hex.end(), ::isspace), hex.end());
                if (hex.empty() || !std::all_of(hex.begin(), hex.end(), ::isxdigit)) {
                    throw std::runtime_error("Invalid hexadecimal value: " + imm_str);
                }
                return std::stoll(hex, nullptr, 16);
            }
            std::string trimmed = imm_str;
            trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), ::isspace), trimmed.end());
            if (trimmed.empty()) {
                throw std::runtime_error("Invalid decimal value: " + imm_str);
            }
            bool is_negative = (!trimmed.empty() && trimmed[0] == '-');
            size_t start = is_negative ? 1 : 0;
            if (!std::all_of(trimmed.begin() + start, trimmed.end(), ::isdigit)) {
                throw std::runtime_error("Invalid decimal value: " + imm_str);
            }
            return std::stoll(trimmed, nullptr, 10);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid immediate value: " + imm_str + " (" + e.what() + ")");
        }
    };

    for (int i = 0; i < lines.size(); ++i) {
        std::string line = lines[i].toStdString();
        std::string trimmed_line = line;
        trimmed_line.erase(std::remove_if(trimmed_line.begin(), trimmed_line.end(), ::isspace), trimmed_line.end());
        if (trimmed_line.empty()) continue;

        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string label = trimmed_line.substr(0, colon_pos);
            label.erase(std::remove_if(label.begin(), label.end(), ::isspace), label.end());
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
                        if (tokens[0] == "li" && tokens.size() >= 3) {
                            long long imm = parseImmediate(tokens[2]);
                            if (imm >= -2048 && imm <= 2047) {
                                pc += 4;
                            } else {
                                pc += 8;
                            }
                        } else {
                            pc += 4;
                        }
                    } else if (tokens[0] == ".word") {
                        pc += 4;
                    } else if (tokens[0] == ".half") {
                        pc += 2;
                    } else if (tokens[0] == ".byte") {
                        pc += 1;
                    } else if (tokens[0] == ".org") {
                        if (tokens.size() == 2) {
                            long long address = parseImmediate(tokens[1]);
                            if (address > 0xFFFFFFFF) {
                                throw std::runtime_error("Invalid .org address: " + tokens[1]);
                            }
                            pc = static_cast<int>(address);
                            qDebug() << ".org address set to:" << pc;
                        }
                    } else if (tokens[0] == ".align") {
                        if (tokens.size() == 2) {
                            long long n = parseImmediate(tokens[1]);
                            if (n > 31) {
                                throw std::runtime_error("Invalid alignment value: " + tokens[1]);
                            }
                            int alignment = 1 << n;
                            pc = ((pc + alignment - 1) / alignment) * alignment;
                        }
                    }
                }
            }
            continue;
        }

        std::vector<std::string> tokens = tokenize(line);
        if (!tokens.empty()) {
            if (instrMap.find(tokens[0]) != instrMap.end()) {
                if (tokens[0] == "li" && tokens.size() >= 3) {
                    long long imm = parseImmediate(tokens[2]);
                    if (imm >= -2048 && imm <= 2047) {
                        pc += 4;
                    } else {
                        pc += 8;
                    }
                } else {
                    pc += 4;
                }
            } else if (tokens[0] == ".word") {
                pc += 4;
            } else if (tokens[0] == ".half") {
                pc += 2;
            } else if (tokens[0] == ".byte") {
                pc += 1;
            } else if (tokens[0] == ".org") {
                if (tokens.size() == 2) {
                    long long address = parseImmediate(tokens[1]);
                    if (address > 0xFFFFFFFF) {
                        throw std::runtime_error("Invalid .org address: " + tokens[1]);
                    }
                    pc = static_cast<int>(address);
                    qDebug() << ".org address set to:" << pc;
                }
            } else if (tokens[0] == ".align") {
                if (tokens.size() == 2) {
                    long long n = parseImmediate(tokens[1]);
                    if (n > 31) {
                        throw std::runtime_error("Invalid alignment value: " + tokens[1]);
                    }
                    int alignment = 1 << n;
                    pc = ((pc + alignment - 1) / alignment) * alignment;
                }
            } else {
                qDebug() << "Invalid instruction or directive at line" << i << ":" << QString::fromStdString(line);
            }
        }
    }
}

void MainWindow::storeInMemory(int value, int size, int& address) {
    if (address + size > static_cast<int>(memory.size())) {
        memory.resize(address + size, 0);
    }
    for (int i = 0; i < size; ++i) {
        memory[address + i] = (value >> (i * 8)) & 0xFF;
    }
    address += size;
}

void MainWindow::saveToBinaryFile(const std::string& filename) {
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open file for writing:" << QString::fromStdString(filename);
        throw std::runtime_error("Failed to open file: " + filename);
    }

    if (maxAddress > memoryArray.size()) {
        maxAddress = memoryArray.size();
    }
    if (maxAddress > 0) {
        file.write(reinterpret_cast<const char*>(memoryArray.data()), maxAddress);
    }

    file.close();
    qDebug() << "Binary file saved:" << QString::fromStdString(filename) << "with" << maxAddress << "bytes";
}

void MainWindow::on_compile_btn_clicked() {
    static int compileCount = 0;
    qDebug() << "Compile button clicked, count:" << ++compileCount;
    if (isCompiling) {
        qDebug() << "Compilation is in progress.";
        return;
    }
    isCompiling = true;
    ui->compile_btn->setEnabled(false);

    QString fullText = ui->code_field->toPlainText();
    QStringList lines = fullText.split("\n", Qt::SkipEmptyParts);
    QStringList binaryOutput;

    memoryArray.assign(1 << 18, 0);
    maxAddress = 0;
    currentAddress = 0;
    parseLabels();

    int pc = 0;
    memory.clear();
    memory.resize(1024 * 1024, 0);

    for (const QString& line : lines) {
        std::string stdLine = line.toStdString();
        std::vector<std::string> tokens = tokenize(stdLine);
        std::string binaryInstruction;
        try {
            binaryInstruction = assemble(stdLine, pc);
            if (!binaryInstruction.empty()) {
                std::vector<std::string> instructions;
                size_t pos = 0;
                std::string delimiter = "\n";
                while ((pos = binaryInstruction.find(delimiter)) != std::string::npos) {
                    instructions.push_back(binaryInstruction.substr(0, pos));
                    binaryInstruction.erase(0, pos + delimiter.length());
                }
                if (!binaryInstruction.empty()) {
                    instructions.push_back(binaryInstruction);
                }

                for (const auto& instr : instructions) {
                    if (instr.length() == 32 || instr.length() == 16 || instr.length() == 8) {
                        binaryOutput.append(QString::fromStdString(instr));
                        qDebug() << "Binary output for line:" << line << ": " << QString::fromStdString(instr);
                    }
                }
            }
        } catch (const std::exception& e) {
            binaryOutput.append(QString("Error in line: %1\n%2").arg(line, QString::fromStdString(e.what())));
            qDebug() << "Error in line:" << line << ": " << QString::fromStdString(e.what());
        }
    }

    try {
        saveToBinaryFile("output.bin");
    } catch (const std::exception& e) {
        binaryOutput.append(QString("Error saving binary file: %1").arg(QString::fromStdString(e.what())));
        qDebug() << "Error saving binary file:" << QString::fromStdString(e.what());
    }

    if (ui->output_field) {
        ui->output_field->setPlainText(binaryOutput.join("\n").trimmed());
    } else {
        QMessageBox::information(this, "Assembly Output", binaryOutput.join("\n"));
    }

    isCompiling = false;
    ui->compile_btn->setEnabled(true);

    // Load binary data and show in ram_table
    QFile file("output.bin");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open output.bin";
        return;
    }

    QByteArray binaryData = file.readAll();
    file.close();

    const int maxAddress = 0xFFFF + 1;

    QStandardItemModel* model = new QStandardItemModel();
    model->setRowCount(maxAddress);
    model->setColumnCount(2);

    model->setHeaderData(0, Qt::Horizontal, "Address");
    model->setHeaderData(1, Qt::Horizontal, "Binary Data");

    for (int i = 0; i < maxAddress; ++i) {
        QString binStr;

        if (i < binaryData.size()) {
            uint8_t byte = static_cast<uint8_t>(binaryData[i]);
            for (int bit = 7; bit >= 0; --bit) {
                binStr += ((byte & (1 << bit)) ? '1' : '0');
            }
        } else {
            binStr = "00000000";
        }

        QString addrStr = QString("0x%1").arg(i, 4, 16, QLatin1Char('0'));

        QStandardItem* addrItem = new QStandardItem(addrStr);
        addrItem->setTextAlignment(Qt::AlignCenter);

        QStandardItem* binItem = new QStandardItem(binStr);
        binItem->setTextAlignment(Qt::AlignCenter);

        model->setItem(i, 0, addrItem);
        model->setItem(i, 1, binItem);
    }

    ui->ram_table->setModel(model);
    ui->ram_table->verticalHeader()->setVisible(false);
    ui->ram_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->ram_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->ram_table->horizontalHeader()->resizeSection(0, 100);
    ui->ram_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    //ui->ram_table->setEditTriggers(QAbstractItemView::NoEditTriggers);


    //general purpose set
    QStandardItemModel* generalModel = new QStandardItemModel();
    generalModel->setRowCount(32);
    generalModel->setColumnCount(2);

    generalModel->setHeaderData(0, Qt::Horizontal, "Address");
    generalModel->setHeaderData(1, Qt::Horizontal, "Binary Data");

    for (int i = 0; i < 32; ++i) {
        QString addrStr = QString("x%1").arg(i, 0, 10);

        QString binStr = QString("00000000000000000000000000000000"); // 32 بیت صفر

        QStandardItem* addrItem = new QStandardItem(addrStr);
        addrItem->setTextAlignment(Qt::AlignCenter);

        QStandardItem* binItem = new QStandardItem(binStr);
        binItem->setTextAlignment(Qt::AlignCenter);

        generalModel->setItem(i, 0, addrItem);
        generalModel->setItem(i, 1, binItem);
    }

    ui->general_table->setModel(generalModel);

    ui->general_table->verticalHeader()->setVisible(false);
    //ui->general_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->general_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->general_table->horizontalHeader()->resizeSection(0, 100);

    ui->general_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->general_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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
    qDebug() << "to_bin(" << val << ", " << bits << "): " << QString::fromStdString(res);
    return res;
}

std::vector<std::string> MainWindow::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    bool inParentheses = false;

    std::string trimmed_line = line;
    size_t start = trimmed_line.find_first_not_of(" \t");
    if (start == std::string::npos) {
        return tokens;
    }
    trimmed_line.erase(0, start);
    size_t end = trimmed_line.find_last_not_of(" \t");
    if (end != std::string::npos) {
        trimmed_line.erase(end + 1);
    }

    if (trimmed_line.empty()) {
        return tokens;
    }

    size_t colon_pos = trimmed_line.find(':');
    if (colon_pos != std::string::npos) {
        std::string label = trimmed_line.substr(0, colon_pos);
        label.erase(std::remove_if(label.begin(), label.end(), ::isspace), label.end());
        if (!label.empty()) {
            tokens.push_back(label + ":");
        }
        if (colon_pos + 1 < trimmed_line.length()) {
            trimmed_line = trimmed_line.substr(colon_pos + 1);
            start = trimmed_line.find_first_not_of(" \t");
            if (start == std::string::npos) {
                return tokens;
            }
            trimmed_line.erase(0, start);
        } else {
            qDebug() << "Tokens for line:" << QString::fromStdString(line) << QString::number(tokens.size()) << (tokens.empty() ? "" : QString::fromStdString(tokens.back()));
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
             std::isspace(trimmed_line[inst.length()]) ||
             trimmed_line[inst.length()] == ',')) {
            tokens.push_back(inst);
            i = inst.length();
            instruction_found = true;
            break;
        }
    }

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

    if (!instruction_found && !trimmed_line.empty()) {
        tokens.push_back(trimmed_line);
        qDebug() << "Tokens for line:" << QString::fromStdString(line) << QString::number(tokens.size()) << (tokens.empty() ? "" : QString::fromStdString(tokens.back()));
        return tokens;
    }

    token.clear();
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
            }
            continue;
        }

        if ((c == ',' || std::isspace(c)) && !inParentheses) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            continue;
        }

        token += c;
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }

    tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [](const std::string& token) { return token.empty(); }), tokens.end());

    qDebug() << "Tokens for line:" << QString::fromStdString(line) << QString::number(tokens.size()) << (tokens.empty() ? "" : QString::fromStdString(tokens[0]));
    return tokens;
}

std::string MainWindow::assemble(const std::string& line, int& currentPC) {
    std::string binaryInstruction;
    std::vector<std::string> tokens = tokenize(line);
    if (tokens.empty()) {
        qDebug() << "Empty tokens for line:" << QString::fromStdString(line);
        return "";
    }

    size_t instruction_start = 0;
    if (tokens[0].back() == ':') {
        instruction_start = 1;
        if (tokens.size() == 1) {
            qDebug() << "Only label found in line:" << QString::fromStdString(line);
            return "";
        }
    }

    if (instruction_start >= tokens.size()) {
        qDebug() << "No instruction after label in line:" << QString::fromStdString(line);
        return "";
    }

    std::string inst = tokens[instruction_start];
    if (instrMap.find(inst) == instrMap.end() && inst != ".org" && inst != ".word" &&
        inst != ".half" && inst != ".byte" && inst != ".align") {
        throw std::runtime_error("Instruction or directive not found: " + inst);
    }

    auto parseImmediate = [&](const std::string& imm_str) -> long long {
        try {
            if (imm_str.length() == 3 && imm_str[0] == '\'' && imm_str[2] == '\'') {
                if (inst != ".byte") {
                    throw std::runtime_error("Character literals like \"" + imm_str + "\" are only allowed with .byte directive");
                }
                char c = imm_str[1];
                return static_cast<long long>(c);
            }
            if (imm_str.find("0x") == 0 || imm_str.find("0X") == 0) {
                std::string hex = imm_str.substr(2);
                hex.erase(std::remove_if(hex.begin(), hex.end(), ::isspace), hex.end());
                if (hex.empty() || !std::all_of(hex.begin(), hex.end(), ::isxdigit)) {
                    throw std::runtime_error("Invalid hexadecimal value: " + imm_str);
                }
                qDebug() << "Parsing hex:" << QString::fromStdString(hex);
                return std::stoll(hex, nullptr, 16);
            }
            if (imm_str.find("0b") == 0 || imm_str.find("0B") == 0) {
                std::string binary = imm_str.substr(2);
                binary.erase(std::remove_if(binary.begin(), binary.end(), ::isspace), binary.end());
                if (binary.empty() || !std::all_of(binary.begin(), binary.end(), [](char c) { return c == '0' || c == '1'; })) {
                    throw std::runtime_error("Invalid binary value: " + imm_str);
                }
                return std::stoll(binary, nullptr, 2);
            }
            std::string trimmed = imm_str;
            trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), ::isspace), trimmed.end());
            if (trimmed.empty()) {
                throw std::runtime_error("Invalid decimal value: " + imm_str);
            }
            bool is_negative = (!trimmed.empty() && trimmed[0] == '-');
            size_t start = is_negative ? 1 : 0;
            if (!std::all_of(trimmed.begin() + start, trimmed.end(), ::isdigit)) {
                throw std::runtime_error("Invalid decimal value: " + imm_str);
            }
            return std::stoll(trimmed, nullptr, 10);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid immediate value: " + imm_str + " (" + std::string(e.what()) + ")");
        }
    };

    try {
        if (inst == ".org") {
            if (tokens.size() != instruction_start + 2) {
                throw std::runtime_error(".org requires 1 argument, got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            long long value = parseImmediate(tokens[instruction_start + 1]);
            if (value > 0xFFFFFFFF) {
                throw std::runtime_error("Invalid .org address: " + tokens[instruction_start + 1]);
            }
            currentPC = static_cast<int>(value);
            currentAddress = static_cast<int>(value);
            maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            return "";
        }
        else if (inst == ".word") {
            if (tokens.size() != instruction_start + 2) {
                throw std::runtime_error(".word requires 1 argument, got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            long long value = parseImmediate(tokens[instruction_start + 1]);
            storeInMemory(static_cast<int>(value), 4, currentAddress);
            binaryInstruction = to_bin(static_cast<int>(value), 32);
            if (currentAddress + 4 <= memoryArray.size()) {
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                currentAddress += 4;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range for .word: " + std::to_string(currentAddress));
            }
            currentPC += 4;
            return binaryInstruction;
        }
        else if (inst == ".half") {
            if (tokens.size() != instruction_start + 2) {
                throw std::runtime_error(".half requires 1 argument, got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            long long value = parseImmediate(tokens[instruction_start + 1]);
            if (value > 65535 || value < -32768) {
                throw std::runtime_error("Value out of range for .half: " + std::to_string(value));
            }
            storeInMemory(static_cast<int>(value), 2, currentAddress);
            binaryInstruction = to_bin(static_cast<int>(value), 16);
            if (currentAddress + 2 <= memoryArray.size()) {
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                currentAddress += 2;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range for .half: " + std::to_string(currentAddress));
            }
            currentPC += 2;
            return binaryInstruction;
        }
        else if (inst == ".byte") {
            if (tokens.size() != instruction_start + 2) {
                throw std::runtime_error(".byte requires 1 argument, got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            long long value = parseImmediate(tokens[instruction_start + 1]);
            if (value > 255 || value < -128) {
                throw std::runtime_error("Value out of range for .byte: " + std::to_string(value));
            }
            storeInMemory(static_cast<int>(value), 1, currentAddress);
            binaryInstruction = to_bin(static_cast<int>(value), 8);
            if (currentAddress + 1 <= memoryArray.size()) {
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                currentAddress += 1;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range for .byte: " + std::to_string(currentAddress));
            }
            currentPC += 1;
            return binaryInstruction;
        }
        else if (inst == ".align") {
            if (tokens.size() != instruction_start + 2) {
                throw std::runtime_error(".align requires 1 argument, got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            long long n = parseImmediate(tokens[instruction_start + 1]);
            if (n > 31) {
                throw std::runtime_error("Invalid alignment value: " + tokens[instruction_start + 1]);
            }
            int alignment = 1 << n;
            int newAddress = ((currentPC + alignment - 1) / alignment) * alignment;
            currentPC = newAddress;
            currentAddress = newAddress;
            maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            return "";
        }

        InstrInfo info = instrMap[inst];

        if (inst == "nop") {
            if (tokens.size() != instruction_start + 1) {
                throw std::runtime_error("nop expects no arguments, got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            std::string rd_val = to_bin(0, 5);
            std::string rs1_val = to_bin(0, 5);
            std::string imm_val = to_bin(0, 12);
            binaryInstruction = imm_val + rs1_val + "000" + rd_val + "0010011";
            if (currentAddress + 4 <= memoryArray.size()) {
                uint32_t value = 0;
                for (char bit : binaryInstruction) {
                    value = (value << 1) | (bit == '1' ? 1 : 0);
                }
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                currentAddress += 4;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
            }
            currentPC += 4;
        }
        else if (inst == "li") {
            if (tokens.size() != instruction_start + 3) {
                throw std::runtime_error("li expects 2 arguments (rd, imm), got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            int rd = regMap.at(tokens[instruction_start + 1]);
            long long imm = parseImmediate(tokens[instruction_start + 2]);
            std::string rd_val = to_bin(rd, 5);

            if (imm >= -2048 && imm <= 2047) {
                std::string imm_val = to_bin(static_cast<int>(imm), 12);
                std::string rs1_val = to_bin(0, 5);
                binaryInstruction = imm_val + rs1_val + "000" + rd_val + "0010011";
                if (currentAddress + 4 <= memoryArray.size()) {
                    uint32_t value = 0;
                    for (char bit : binaryInstruction) {
                        value = (value << 1) | (bit == '1' ? 1 : 0);
                    }
                    memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                    memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                    memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                    memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                    currentAddress += 4;
                    maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
                } else {
                    throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
                }
                currentPC += 4;
            } else {
                int upper_bits = upper(imm);
                int lower_bits = lower(imm);
                std::string lui_imm = to_bin(upper_bits, 20);
                std::string lui_instruction = lui_imm + rd_val + "0110111";
                std::string addi_imm = to_bin(lower_bits, 12);
                std::string rs1_val = rd_val;
                std::string addi_instruction = addi_imm + rs1_val + "000" + rd_val + "0010011";
                binaryInstruction = lui_instruction + "\n" + addi_instruction;
                if (currentAddress + 8 <= memoryArray.size()) {
                    uint32_t value = 0;
                    for (char bit : lui_instruction) {
                        value = (value << 1) | (bit == '1' ? 1 : 0);
                    }
                    memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                    memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                    memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                    memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                    currentAddress += 4;
                    value = 0;
                    for (char bit : addi_instruction) {
                        value = (value << 1) | (bit == '1' ? 1 : 0);
                    }
                    memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                    memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                    memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                    memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                    currentAddress += 4;
                    maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
                } else {
                    throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
                }
                currentPC += 8;
            }
        }
        else if (inst == "mv") {
            if (tokens.size() != instruction_start + 3) {
                throw std::runtime_error("mv expects 2 arguments (rd, rs), got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            int rd = regMap.at(tokens[instruction_start + 1]);
            int rs = regMap.at(tokens[instruction_start + 2]);
            std::string rd_val = to_bin(rd, 5);
            std::string rs1_val = to_bin(rs, 5);
            std::string imm_val = to_bin(0, 12);
            binaryInstruction = imm_val + rs1_val + "000" + rd_val + "0010011";
            if (currentAddress + 4 <= memoryArray.size()) {
                uint32_t value = 0;
                for (char bit : binaryInstruction) {
                    value = (value << 1) | (bit == '1' ? 1 : 0);
                }
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                currentAddress += 4;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
            }
            currentPC += 4;
        }
        else if (inst == "not") {
            if (tokens.size() != instruction_start + 3) {
                throw std::runtime_error("not expects 2 arguments (rd, rs), got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            int rd = regMap.at(tokens[instruction_start + 1]);
            int rs = regMap.at(tokens[instruction_start + 2]);
            std::string rd_val = to_bin(rd, 5);
            std::string rs1_val = to_bin(rs, 5);
            std::string imm_val = to_bin(-1, 12);
            binaryInstruction = imm_val + rs1_val + "100" + rd_val + "0010011";
            if (currentAddress + 4 <= memoryArray.size()) {
                uint32_t value = 0;
                for (char bit : binaryInstruction) {
                    value = (value << 1) | (bit == '1' ? 1 : 0);
                }
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                currentAddress += 4;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
            }
            currentPC += 4;
        }
        else if (inst == "neg") {
            if (tokens.size() != instruction_start + 3) {
                throw std::runtime_error("neg expects 2 arguments (rd, rs), got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            int rd = regMap.at(tokens[instruction_start + 1]);
            int rs = regMap.at(tokens[instruction_start + 2]);
            std::string rd_val = to_bin(rd, 5);
            std::string rs1_val = to_bin(0, 5);
            std::string rs2_val = to_bin(rs, 5);
            binaryInstruction = "0100000" + rs2_val + rs1_val + "000" + rd_val + "0110011";
            if (currentAddress + 4 <= memoryArray.size()) {
                uint32_t value = 0;
                for (char bit : binaryInstruction) {
                    value = (value << 1) | (bit == '1' ? 1 : 0);
                }
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                currentAddress += 4;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
            }
            currentPC += 4;
        }
        else if (inst == "add" || inst == "sub" || inst == "xor" || inst == "or" || inst == "and" ||
                 inst == "sll" || inst == "srl" || inst == "sra" || inst == "slt" || inst == "sltu" ||
                 inst == "mul" || inst == "mulh" || inst == "div" || inst == "rem") {
            if (tokens.size() != instruction_start + 4) {
                throw std::runtime_error(inst + " expects 3 arguments (rd, rs1, rs2), got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            int rd = regMap.at(tokens[instruction_start + 1]);
            int rs1 = regMap.at(tokens[instruction_start + 2]);
            int rs2 = regMap.at(tokens[instruction_start + 3]);

            std::string rd_val = to_bin(rd, 5);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rs2_val = to_bin(rs2, 5);

            binaryInstruction = info.funct7 + rs2_val + rs1_val + info.funct3 + rd_val + info.opcode;
            if (currentAddress + 4 <= memoryArray.size()) {
                uint32_t value = 0;
                for (char bit : binaryInstruction) {
                    value = (value << 1) | (bit == '1' ? 1 : 0);
                }
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                currentAddress += 4;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
            }
            currentPC += 4;
        }
        else if (inst == "addi" || inst == "xori") {
            if (tokens.size() != instruction_start + 4) {
                throw std::runtime_error(inst + " expects 3 arguments (rd, rs1, imm), got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            int rd = regMap.at(tokens[instruction_start + 1]);
            int rs1 = regMap.at(tokens[instruction_start + 2]);
            long long imm = parseImmediate(tokens[instruction_start + 3]);

            if (imm < -2048 || imm > 2047) {
                throw std::runtime_error("Immediate value out of range for " + inst + ": " + std::to_string(imm));
            }

            std::string imm_val = to_bin(static_cast<int>(imm), 12);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rd_val = to_bin(rd, 5);

            binaryInstruction = imm_val + rs1_val + info.funct3 + rd_val + info.opcode;
            if (currentAddress + 4 <= memoryArray.size()) {
                uint32_t value = 0;
                for (char bit : binaryInstruction) {
                    value = (value << 1) | (bit == '1' ? 1 : 0);
                }
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                currentAddress += 4;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
            }
            currentPC += 4;
        }
        else if (inst == "lh" || inst == "lw" || inst == "jalr") {
            if (tokens.size() != instruction_start + 3) {
                throw std::runtime_error(inst + " expects 2 arguments (rd, imm(rs1)), got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            int rd = regMap.at(tokens[instruction_start + 1]);
            std::string imm_rs1 = tokens[instruction_start + 2];
            size_t pos = imm_rs1.find('(');
            if (pos == std::string::npos || pos == 0 || imm_rs1.back() != ')') {
                throw std::runtime_error("Invalid imm(rs1) format: " + imm_rs1);
            }
            std::string imm_str = imm_rs1.substr(0, pos);
            long long imm = parseImmediate(imm_str);
            std::string rs1_str = imm_rs1.substr(pos + 1, imm_rs1.length() - pos - 2);
            if (regMap.find(rs1_str) == regMap.end()) {
                throw std::runtime_error("Invalid register: " + rs1_str);
            }
            int rs1 = regMap.at(rs1_str);

            if (imm < -2048 || imm > 2047) {
                throw std::runtime_error("Immediate value out of range for " + inst + ": " + std::to_string(imm));
            }

            std::string imm_val = to_bin(static_cast<int>(imm), 12);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rd_val = to_bin(rd, 5);

            binaryInstruction = imm_val + rs1_val + info.funct3 + rd_val + info.opcode;
            if (currentAddress + 4 <= memoryArray.size()) {
                uint32_t value = 0;
                for (char bit : binaryInstruction) {
                    value = (value << 1) | (bit == '1' ? 1 : 0);
                }
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                currentAddress += 4;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
            }
            currentPC += 4;
        }
        else if (inst == "sh" || inst == "sw") {
            if (tokens.size() != instruction_start + 3) {
                throw std::runtime_error(inst + " expects 2 arguments (rs2, imm(rs1)), got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            int rs2 = regMap.at(tokens[instruction_start + 1]);
            std::string imm_rs1 = tokens[instruction_start + 2];
            size_t pos = imm_rs1.find('(');
            if (pos == std::string::npos || pos == 0 || imm_rs1.back() != ')') {
                throw std::runtime_error("Invalid imm(rs1) format: " + imm_rs1);
            }
            std::string imm_str = imm_rs1.substr(0, pos);
            long long imm = parseImmediate(imm_str);
            std::string rs1_str = imm_rs1.substr(pos + 1, imm_rs1.length() - pos - 2);
            if (regMap.find(rs1_str) == regMap.end()) {
                throw std::runtime_error("Invalid register: " + rs1_str);
            }
            int rs1 = regMap.at(rs1_str);

            if (imm < -2048 || imm > 2047) {
                throw std::runtime_error("Immediate value out of range for " + inst + ": " + std::to_string(imm));
            }

            std::string imm_val = to_bin(static_cast<int>(imm), 12);
            std::string immHi = imm_val.substr(0, 7);
            std::string immLo = imm_val.substr(7, 5);
            std::string rs1_val = to_bin(rs1, 5);
            std::string rs2_val = to_bin(rs2, 5);

            binaryInstruction = immHi + rs2_val + rs1_val + info.funct3 + immLo + info.opcode;
            if (currentAddress + 4 <= memoryArray.size()) {
                uint32_t value = 0;
                for (char bit : binaryInstruction) {
                    value = (value << 1) | (bit == '1' ? 1 : 0);
                }
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                currentAddress += 4;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
            }
            currentPC += 4;
        }
        else if (inst == "beq" || inst == "bne" || inst == "blt" || inst == "bge" || inst == "bltu" || inst == "bgeu") {
            if (tokens.size() != instruction_start + 4) {
                throw std::runtime_error(inst + " expects 3 arguments (rs1, rs2, label/imm), got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            int rs1 = regMap.at(tokens[instruction_start + 1]);
            int rs2 = regMap.at(tokens[instruction_start + 2]);
            std::string target = tokens[instruction_start + 3];
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
                qDebug() << inst.c_str() << ": targetPC = " << targetPC << ", currentPC = " << currentPC << ", imm = " << imm;
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
            std::string imm11 = imm_val.substr(7, 1);

            binaryInstruction = imm12 + imm105 + rs2_val + rs1_val + info.funct3 + imm41 + imm11 + info.opcode;
            if (currentAddress + 4 <= memoryArray.size()) {
                uint32_t value = 0;
                for (char bit : binaryInstruction) {
                    value = (value << 1) | (bit == '1' ? 1 : 0);
                }
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                currentAddress += 4;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
            }
            currentPC += 4;
        }
        else if (inst == "jal") {
            if (tokens.size() != instruction_start + 3) {
                throw std::runtime_error("jal expects 2 arguments (rd, label/imm), got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            int rd = regMap.at(tokens[instruction_start + 1]);
            std::string target = tokens[instruction_start + 2];
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


            std::string imm_val = to_bin(imm >> 0, 20); // شیفت به راست برای حذف بیت صفر
            std::string rd_val = to_bin(rd, 5);
            std::string imm20 = imm_val.substr(0, 1);      // imm[20]
            std::string imm10_1 = imm_val.substr(1, 10);   // imm[10:1]
            std::string imm11 = imm_val.substr(11, 1);     // imm[11]
            std::string imm19_12 = imm_val.substr(12, 8);  // imm[19:12]

            binaryInstruction = imm20 + imm10_1 + imm11 + imm19_12 + rd_val + info.opcode;
            if (currentAddress + 4 <= memoryArray.size()) {
                uint32_t value = 0;
                for (char bit : binaryInstruction) {
                    value = (value << 1) | (bit == '1' ? 1 : 0);
                }
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                currentAddress += 4;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
            }
            currentPC += 4;
        }
        else if (inst == "lui" || inst == "auipc") {
            if (tokens.size() != instruction_start + 3) {
                throw std::runtime_error(inst + " expects 2 arguments (rd, imm), got " + std::to_string(tokens.size() - instruction_start - 1));
            }
            int rd = regMap.at(tokens[instruction_start + 1]);
            long long imm = parseImmediate(tokens[instruction_start + 2]);

            if (imm > 1048575) {
                throw std::runtime_error("Immediate value out of range for " + inst + ": " + std::to_string(imm));
            }

            std::string imm_val = to_bin(static_cast<int>(imm), 20);
            std::string rd_val = to_bin(rd, 5);

            binaryInstruction = imm_val + rd_val + info.opcode;
            if (currentAddress + 4 <= memoryArray.size()) {
                uint32_t value = 0;
                for (char bit : binaryInstruction) {
                    value = (value << 1) | (bit == '1' ? 1 : 0);
                }
                memoryArray[currentAddress + 0] = static_cast<uint8_t>(value & 0xFF);
                memoryArray[currentAddress + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
                memoryArray[currentAddress + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
                memoryArray[currentAddress + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
                currentAddress += 4;
                maxAddress = std::max(maxAddress, static_cast<size_t>(currentAddress));
            } else {
                throw std::runtime_error("Memory address out of range: " + std::to_string(currentAddress));
            }
            currentPC += 4;
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(e.what());
    }

    return binaryInstruction;
}

void MainWindow::on_run_btn_clicked()
{

}

int MainWindow::binaryToSignedInt(const QString& binStr) {
    int bitWidth = binStr.length();
    bool ok = false;

    quint64 unsignedVal = binStr.toULongLong(&ok, 2);
    if (!ok) {
        return 0;
    }

    bool isNegative = (binStr[0] == '1');

    if (isNegative) {
        qint64 signedVal = static_cast<qint64>(unsignedVal) - (1LL << bitWidth);
        return static_cast<int>(signedVal);

    }
    else {
        return static_cast<int>(unsignedVal);
    }
}


void MainWindow::on_clock_btn_clicked()
{
    QString PC, MAR, IR, UPCODE, FUNCT3, GAR, FUNCT7;
    static int Z = 0,P = 0,N = 0;
    QString SC = ui->sc_label->text();

    if(SC == "\\- \\- \\-"){
        SC_n = -1;
    }
    else if(SC_n != -1){
        SC_n = SC.toInt(nullptr, 2);
    }

    SC_n++;
    SC = QString("%1").arg(SC_n, 3, 2, QChar('0'));
    ui->sc_label->setText(SC);

    if(SC_n == 0){
        PC = ui->pc_label->text();
        MAR = PC;
        ui->mar_label->setText(MAR);
    }

    if(SC_n == 1){
        MAR = ui->mar_label->text();
        int marAddr = MAR.toInt(nullptr, 2);

        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->ram_table->model());

        for (int offset = 3; offset >= 0; --offset) {
            int targetAddr = marAddr + offset;
            QString addrStr = QString("0x%1").arg(targetAddr, 4, 16, QChar('0'));

            for (int row = 0; row < model->rowCount(); ++row) {
                QString addrInTable = model->item(row, 0)->text();
                if (addrInTable == addrStr) {
                    IR += model->item(row, 1)->text();
                    break;
                }
            }
        }

        ui->ir_label->setText(IR);

        PC = ui->pc_label->text();
        int PC_n = PC.toInt(nullptr, 2);
        PC_n += 4;
        PC = QString("%1").arg(PC_n, 16, 2, QChar('0'));
        ui->pc_label->setText(PC);
    }

    if(SC_n == 2){
        IR = ui->ir_label->text();
        UPCODE = IR.mid(25, 7);
        ui->upcode_label->setText(UPCODE);
    }

    UPCODE = ui->upcode_label->text();

    if((UPCODE == R || UPCODE == I1 || UPCODE == I2 || UPCODE == I3 || UPCODE == S || UPCODE == B) && SC_n == 3){
        IR = ui->ir_label->text();
        FUNCT3 = IR.mid(17, 3);
        ui->funct3_label->setText(FUNCT3);
    }

    if((UPCODE == R || UPCODE == I1 || UPCODE == I2 || UPCODE == I3 || UPCODE == U1 || UPCODE == U2 || UPCODE == J) && SC_n == 3){
        IR = ui->ir_label->text();
        GAR = IR.mid(20, 5);
        ui->gar_label->setText(GAR);
    }

    if(UPCODE == R && SC_n == 3){
        IR = ui->ir_label->text();
        FUNCT7 = IR.mid(0, 7);
        ui->funct7_label->setText(FUNCT7);
    }

    if((UPCODE == I1 || UPCODE == I2 || UPCODE == I3) && SC_n == 3){
        IR = ui->ir_label->text();
        QString IMM = IR.mid(0, 12);
        ui->imm_label->setText(IMM);
    }

    if(UPCODE == S && SC_n == 3){
        IR = ui->ir_label->text();
        QString IMM = IR.mid(0, 7);
        IMM += IR.mid(20, 5);
        ui->imm_label->setText(IMM);
    }

    if(UPCODE == B && SC_n == 3){
        IR = ui->ir_label->text();
        QString IMM = "0000000000000";
        IMM.replace(5, 6, IR.mid(0, 6));
        IMM[12] = IR[0];
        IMM.replace(1, 4, IR.mid(20, 4));
        IMM[11] = IR[24];
        ui->imm_label->setText(IMM);
    }

    if((UPCODE == U1 || UPCODE == U2) && SC_n == 3){
        IR = ui->ir_label->text();
        QString IMM = "00000000000000000000000000000000";
        IMM.replace(12, 10, IR.mid(0, 10));
        ui->imm_label->setText(IMM);
    }

    if(UPCODE == J && SC_n == 3){
        IR = ui->ir_label->text();
        QString IMM = "000000000000000000000";
        IMM.replace(12, 8, IR.mid(12, 8));
        IMM[11] = IR[11];
        IMM.replace(1, 10, IR.mid(1, 10));
        IMM[20] = IR[0];
        ui->imm_label->setText(IMM);
    }

    if(UPCODE == R && SC_n == 4){
        GAR = ui->gar_label->text();
        IR = ui->ir_label->text();
        FUNCT3 = ui->funct3_label->text();
        FUNCT7 = ui->funct7_label->text();

        QString RS1 = IR.mid(12,5);
        QString RS2 = IR.mid(7,5);

        int addr1 = RS1.toInt(nullptr, 2);
        int addr2 = RS2.toInt(nullptr, 2);
        int garIndex = GAR.toInt(nullptr, 2);

        QString targetRegister1 = QString("x%1").arg(addr1);
        QString targetRegister2 = QString("x%1").arg(addr2);
        QString targetRegisterGAR = QString("x%1").arg(garIndex);

        QStandardItemModel* generalModel = qobject_cast<QStandardItemModel*>(ui->general_table->model());

        for (int row = 0; row < generalModel->rowCount(); ++row) {
            QString addrInTable = generalModel->item(row, 0)->text();
            if (addrInTable == targetRegister1) {
                RS1 = generalModel->item(row, 1)->text();
            }
            if(addrInTable == targetRegister2){
                RS2 = generalModel->item(row, 1)->text();
            }
        }

        //add
        if(FUNCT3 == "000" && FUNCT7 == "0000000"){
            int RS1_V = binaryToSignedInt(RS1);
            int RS2_V = binaryToSignedInt(RS2);
            int answer = RS1_V + RS2_V ;
            QString binaryAnswer = QString("%1").arg(static_cast<quint32>(answer), 32, 2, QChar('0'));

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //sub
        if(FUNCT3 == "000" && FUNCT7 == "0100000"){
            int RS1_V = binaryToSignedInt(RS1);
            int RS2_V = binaryToSignedInt(RS2);
            int answer = RS1_V - RS2_V ;
            QString binaryAnswer = QString("%1").arg(static_cast<quint32>(answer), 32, 2, QChar('0'));

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //xor
        if(FUNCT3 == "100" && FUNCT7 == "0000000"){
            QString binaryAnswer;
            int len = RS1.length();

            for (int i = 0; i < len; ++i) {
                QChar bitA = RS1[i];
                QChar bitB = RS2[i];

                if (bitA != bitB) {
                    binaryAnswer.append('1');
                } else {
                    binaryAnswer.append('0');
                }
            }

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //or
        if(FUNCT3 == "110" && FUNCT7 == "0000000"){
            QString binaryAnswer;
            int len = RS1.length();

            for (int i = 0; i < len; ++i) {
                QChar bitA = RS1[i];
                QChar bitB = RS2[i];

                if (bitA == '1' || bitB == '1') {
                    binaryAnswer.append('1');
                } else {
                    binaryAnswer.append('0');
                }
            }

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //and
        if(FUNCT3 == "111" && FUNCT7 == "0000000"){
            QString binaryAnswer;
            int len = RS1.length();

            for (int i = 0; i < len; ++i) {
                QChar bitA = RS1[i];
                QChar bitB = RS2[i];

                if (bitA == '1' && bitB == '1') {
                    binaryAnswer.append('1');
                } else {
                    binaryAnswer.append('0');
                }
            }

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //sll
        if(FUNCT3 == "001" && FUNCT7 == "0000000"){
            QString RS2_5bits = RS2.mid(RS2.size() - 5, 5);
            unsigned shiftAmount = RS2_5bits.toUInt(nullptr, 2);

            int RS1_V = binaryToSignedInt(RS1);
            int answer = RS1_V << shiftAmount;

            QString binaryAnswer = QString("%1").arg(static_cast<quint32>(answer), 32, 2, QChar('0'));

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //srl
        if(FUNCT3 == "101" && FUNCT7 == "0000000"){
            QString RS2_5bits = RS2.mid(RS2.size() - 5, 5);
            unsigned shiftAmount = RS2_5bits.toUInt(nullptr, 2);

            unsigned int RS1_V = RS1.toUInt(nullptr, 2);
            unsigned int answer = RS1_V >> shiftAmount;

            QString binaryAnswer = QString("%1").arg(static_cast<quint32>(answer), 32, 2, QChar('0'));
            qDebug() << binaryAnswer;

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //sra
        if(FUNCT3 == "101" && FUNCT7 == "0100000"){
            QString RS2_5bits = RS2.mid(RS2.size() - 5, 5);
            unsigned int shiftAmount = RS2_5bits.toUInt(nullptr, 2);

            int RS1_V = binaryToSignedInt(RS1);
            int answer = RS1_V >> shiftAmount;

            QString binaryAnswer = QString("%1").arg(static_cast<quint32>(answer), 32, 2, QChar('0'));

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //mul
        if(FUNCT3 == "000" && FUNCT7 == "0000001"){
            int RS1_V = binaryToSignedInt(RS1);
            int RS2_V = binaryToSignedInt(RS2);

            qint64 fullResult = static_cast<qint64>(RS1_V) * static_cast<qint64>(RS2_V);

            quint32 answer = static_cast<quint32>(fullResult & 0xFFFFFFFF);

            QString binaryAnswer = QString("%1").arg(static_cast<quint32>(answer), 32, 2, QChar('0'));

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //mulh
        if(FUNCT3 == "001" && FUNCT7 == "0000001"){
            int RS1_V = binaryToSignedInt(RS1);
            int RS2_V = binaryToSignedInt(RS2);

            qint64 fullResult = static_cast<qint64>(RS1_V) * static_cast<qint64>(RS2_V);

            quint32 answer = static_cast<quint32>((fullResult >> 32) & 0xFFFFFFFF);

            QString binaryAnswer = QString("%1").arg(static_cast<quint32>(answer), 32, 2, QChar('0'));

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);
                    break;
                }
            }

            SC_n = -1;
        }

        //div
        if(FUNCT3 == "100" && FUNCT7 == "0000001"){
            int RS1_V = binaryToSignedInt(RS1);
            int RS2_V = binaryToSignedInt(RS2);
            int answer;

            if (RS2_V == 0){
                answer = 0;
            }
            else{
                answer = RS1_V / RS2_V;
            }

            QString binaryAnswer = QString("%1").arg(static_cast<quint32>(answer), 32, 2, QChar('0'));

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //rem
        if(FUNCT3 == "110" && FUNCT7 == "0000001"){
            int RS1_V = binaryToSignedInt(RS1);
            int RS2_V = binaryToSignedInt(RS2);
            int answer;

            if (RS2_V == 0){
                answer = RS1_V;
            }
            else{
                answer = RS1_V % RS2_V;
            }

            QString binaryAnswer = QString("%1").arg(static_cast<quint32>(answer), 32, 2, QChar('0'));

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }
    }

    if(UPCODE == R && SC_n == 5){
        GAR = ui->gar_label->text();
        IR = ui->ir_label->text();
        FUNCT3 = ui->funct3_label->text();
        FUNCT7 = ui->funct7_label->text();

        QString RS1 = IR.mid(12,5);
        QString RS2 = IR.mid(7,5);

        int addr1 = RS1.toInt(nullptr, 2);
        int addr2 = RS2.toInt(nullptr, 2);
        int garIndex = GAR.toInt(nullptr, 2);

        QString targetRegister1 = QString("x%1").arg(addr1);
        QString targetRegister2 = QString("x%1").arg(addr2);
        QString targetRegisterGAR = QString("x%1").arg(garIndex);

        QStandardItemModel* generalModel = qobject_cast<QStandardItemModel*>(ui->general_table->model());

        for (int row = 0; row < generalModel->rowCount(); ++row) {
            QString addrInTable = generalModel->item(row, 0)->text();
            if (addrInTable == targetRegister1) {
                RS1 = generalModel->item(row, 1)->text();
            }
            if(addrInTable == targetRegister2){
                RS2 = generalModel->item(row, 1)->text();
            }
        }

        //slt
        if(FUNCT3 == "010" && FUNCT7 == "0000000"){
            int RS1_V = binaryToSignedInt(RS1);
            int RS2_V = binaryToSignedInt(RS2);

            int answer = RS1_V < RS2_V ? 1 : 0;
            QString binaryAnswer = QString("%1").arg(static_cast<quint32>(answer), 32, 2, QChar('0'));

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //sltu
        if(FUNCT3 == "011" && FUNCT7 == "0000000"){
            unsigned RS1_V = RS1.toUInt(nullptr, 2);
            unsigned RS2_V = RS2.toUInt(nullptr, 2);
            int answer = RS1_V < RS2_V ? 1 : 0;
            QString binaryAnswer = QString("%1").arg(static_cast<quint32>(answer), 32, 2, QChar('0'));

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }
    }

    if((UPCODE == I1 || UPCODE == I2 || UPCODE == I3) && (SC_n == 4 || SC_n == 5)){
        GAR = ui->gar_label->text();
        IR = ui->ir_label->text();
        FUNCT3 = ui->funct3_label->text();
        QString IMM = ui->imm_label->text();
        QString RS1 = IR.mid(12,5);

        int addr1 = RS1.toInt(nullptr, 2);
        int garIndex = GAR.toInt(nullptr, 2);
        QString targetRegister1 = QString("x%1").arg(addr1);
        QString targetRegisterGAR = QString("x%1").arg(garIndex);

        QStandardItemModel* generalModel = qobject_cast<QStandardItemModel*>(ui->general_table->model());

        for (int row = 0; row < generalModel->rowCount(); ++row) {
            QString addrInTable = generalModel->item(row, 0)->text();
            if (addrInTable == targetRegister1) {
                RS1 = generalModel->item(row, 1)->text();
                break;
            }
        }

        //addi
        if(UPCODE == I1 && FUNCT3 == "000" && SC_n == 4){
            int IMM_V = binaryToSignedInt(IMM);
            int RS1_V = binaryToSignedInt(RS1);
            int answer = RS1_V + IMM_V;

            QString binaryAnswer = QString("%1").arg(static_cast<quint32>(answer), 32, 2, QChar('0'));

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        if(UPCODE == I2 && (FUNCT3 == "001" || FUNCT3 == "010") && SC_n == 4){
            int IMM_V = binaryToSignedInt(IMM);
            int RS1_V = binaryToSignedInt(RS1);
            int address = RS1_V + IMM_V;

            quint16 address_16bit = static_cast<quint16>(address);

            QString binaryAnswer = QString("%1").arg(address_16bit, 16, 2, QChar('0'));

            ui->mar_label->setText(binaryAnswer);
        }

        //lh
        if(UPCODE == I2 && FUNCT3 == "001" && SC_n == 5){
            QString answer;
            MAR = ui->mar_label->text();
            int marAddr = MAR.toInt(nullptr, 2);

            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->ram_table->model());

            for (int offset = 1; offset >= 0; --offset) {
                int targetAddr = marAddr + offset;
                QString addrStr = QString("0x%1").arg(targetAddr, 4, 16, QChar('0'));

                for (int row = 0; row < model->rowCount(); ++row) {
                    QString addrInTable = model->item(row, 0)->text();
                    if (addrInTable == addrStr) {
                        answer += model->item(row, 1)->text();
                        break;
                    }
                }
            }

            QString binaryAnswer = "0000000000000000" + answer;

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //lw
        if(UPCODE == I2 && FUNCT3 == "010" && SC_n == 5){
            QString answer;
            MAR = ui->mar_label->text();
            int marAddr = MAR.toInt(nullptr, 2);

            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->ram_table->model());

            for (int offset = 3; offset >= 0; --offset) {
                int targetAddr = marAddr + offset;
                QString addrStr = QString("0x%1").arg(targetAddr, 4, 16, QChar('0'));

                for (int row = 0; row < model->rowCount(); ++row) {
                    QString addrInTable = model->item(row, 0)->text();
                    if (addrInTable == addrStr) {
                        answer += model->item(row, 1)->text();
                        break;
                    }
                }
            }

            QString binaryAnswer = answer;

            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }

            SC_n = -1;
        }

        //jalr
        if(UPCODE == I3 && FUNCT3 == "000" && SC_n == 4){
            PC = ui->pc_label->text();
            quint16 PC_V_16bit = static_cast<quint16>(PC.toUInt(nullptr, 2));
            quint32 rd_value = static_cast<quint32>(PC_V_16bit) + 4;

            QString binaryAnswer = QString("%1").arg(rd_value, 32, 2, QChar('0'));


            for (int row = 0; row < generalModel->rowCount(); ++row) {
                QString addrInTable = generalModel->item(row, 0)->text();
                if (addrInTable == targetRegisterGAR) {
                    QStandardItem* item = new QStandardItem(binaryAnswer);
                    item->setTextAlignment(Qt::AlignCenter);
                    generalModel->setItem(row, 1, item);

                    break;
                }
            }
        }
        if(UPCODE == I3 && FUNCT3 == "000" && SC_n == 5){
            PC = ui->pc_label->text();
            int IMM_V = binaryToSignedInt(IMM);
            int RS1_V = binaryToSignedInt(RS1);
            int address = RS1_V + IMM_V;

            quint16 address_16bit = static_cast<quint16>(address);

            QString binaryAnswer = QString("%1").arg(address_16bit, 16, 2, QChar('0'));

            ui->pc_label->setText(binaryAnswer);

            SC_n = -1;
        }

    }

    if(UPCODE == S && (SC_n == 4 || SC_n == 5)){
        IR = ui->ir_label->text();
        FUNCT3 = ui->funct3_label->text();
        QString IMM = ui->imm_label->text();
        QString RS1 = IR.mid(12,5);
        QString RS2 = IR.mid(7,5);

        int addr1 = RS1.toInt(nullptr, 2);
        int addr2 = RS2.toInt(nullptr, 2);

        QString targetRegister1 = QString("x%1").arg(addr1);
        QString targetRegister2 = QString("x%1").arg(addr2);

        QStandardItemModel* generalModel = qobject_cast<QStandardItemModel*>(ui->general_table->model());

        for (int row = 0; row < generalModel->rowCount(); ++row) {
            QString addrInTable = generalModel->item(row, 0)->text();
            if (addrInTable == targetRegister1) {
                RS1 = generalModel->item(row, 1)->text();
            }
            if(addrInTable == targetRegister2){
                RS2 = generalModel->item(row, 1)->text();
            }
        }

        if(SC_n == 4){
            int IMM_V = binaryToSignedInt(IMM);
            int RS1_V = binaryToSignedInt(RS1);
            int address = RS1_V + IMM_V;

            quint16 address_16bit = static_cast<quint16>(address);

            QString binaryAnswer = QString("%1").arg(address_16bit, 16, 2, QChar('0'));

            ui->mar_label->setText(binaryAnswer);
        }

        //sh
        if(FUNCT3 == "001" && SC_n == 5){
            MAR = ui->mar_label->text();
            quint16 MAR_addr = MAR.toUInt(nullptr, 2);

            quint32 RS2_val = RS2.toUInt(nullptr, 2);

            quint8 low_byte = RS2_val & 0xFF;          // [7:0]
            quint8 high_byte = (RS2_val >> 8) & 0xFF;  // [15:8]

            QString addr0 = QString("0x%1").arg(MAR_addr, 4, 16, QLatin1Char('0')).toLower();
            QString addr1 = QString("0x%1").arg(MAR_addr + 1, 4, 16, QLatin1Char('0')).toLower();

            QStandardItemModel* ramModel = qobject_cast<QStandardItemModel*>(ui->ram_table->model());

            for (int row = 0; row < ramModel->rowCount(); ++row) {
                QString rowAddr = ramModel->item(row, 0)->text().toLower();

                if (rowAddr == addr0) {
                    QString val = QString("%1").arg(low_byte, 8, 2, QLatin1Char('0')).toLower();
                    QStandardItem* item = new QStandardItem(val);
                    item->setTextAlignment(Qt::AlignCenter);
                    ramModel->setItem(row, 1, item);
                }

                if (rowAddr == addr1) {
                    QString val = QString("%1").arg(high_byte, 8, 2, QLatin1Char('0')).toLower();
                    QStandardItem* item = new QStandardItem(val);
                    item->setTextAlignment(Qt::AlignCenter);
                    ramModel->setItem(row, 1, item);
                }
            }

            SC_n = -1;
        }

        //sw
        if (FUNCT3 == "010" && SC_n == 5) {
            MAR = ui->mar_label->text();
            quint16 MAR_addr = MAR.toUInt(nullptr, 2);

            quint32 RS2_val = RS2.toUInt(nullptr, 2);

            quint8 byte0 = (RS2_val) & 0xFF;         // [7:0]
            quint8 byte1 = (RS2_val >> 8) & 0xFF;    // [15:8]
            quint8 byte2 = (RS2_val >> 16) & 0xFF;   // [23:16]
            quint8 byte3 = (RS2_val >> 24) & 0xFF;   // [31:24]

            QString addr0 = QString("0x%1").arg(MAR_addr, 4, 16, QLatin1Char('0')).toLower();
            QString addr1 = QString("0x%1").arg(MAR_addr + 1, 4, 16, QLatin1Char('0')).toLower();
            QString addr2 = QString("0x%1").arg(MAR_addr + 2, 4, 16, QLatin1Char('0')).toLower();
            QString addr3 = QString("0x%1").arg(MAR_addr + 3, 4, 16, QLatin1Char('0')).toLower();

            QStandardItemModel* ramModel = qobject_cast<QStandardItemModel*>(ui->ram_table->model());

            for (int row = 0; row < ramModel->rowCount(); ++row) {
                QString rowAddr = ramModel->item(row, 0)->text().toLower();

                if (rowAddr == addr0) {
                    QString val = QString("%1").arg(byte0, 8, 2, QChar('0'));  // 8-bit binary
                    QStandardItem* item = new QStandardItem(val);
                    item->setTextAlignment(Qt::AlignCenter);
                    ramModel->setItem(row, 1, item);
                }
                else if (rowAddr == addr1) {
                    QString val = QString("%1").arg(byte1, 8, 2, QChar('0'));
                    QStandardItem* item = new QStandardItem(val);
                    item->setTextAlignment(Qt::AlignCenter);
                    ramModel->setItem(row, 1, item);
                }
                else if (rowAddr == addr2) {
                    QString val = QString("%1").arg(byte2, 8, 2, QChar('0'));
                    QStandardItem* item = new QStandardItem(val);
                    item->setTextAlignment(Qt::AlignCenter);
                    ramModel->setItem(row, 1, item);
                }
                else if (rowAddr == addr3) {
                    QString val = QString("%1").arg(byte3, 8, 2, QChar('0'));
                    QStandardItem* item = new QStandardItem(val);
                    item->setTextAlignment(Qt::AlignCenter);
                    ramModel->setItem(row, 1, item);
                }
            }

            SC_n = -1;
        }

    }

    if(UPCODE == B && (SC_n == 4 || SC_n == 5)){
        IR = ui->ir_label->text();
        FUNCT3 = ui->funct3_label->text();
        PC = ui->pc_label->text();
        QString IMM = ui->imm_label->text();

        QString RS1 = IR.mid(12,5);
        QString RS2 = IR.mid(7,5);

        int addr1 = RS1.toInt(nullptr, 2);
        int addr2 = RS2.toInt(nullptr, 2);

        QString targetRegister1 = QString("x%1").arg(addr1);
        QString targetRegister2 = QString("x%1").arg(addr2);

        QStandardItemModel* generalModel = qobject_cast<QStandardItemModel*>(ui->general_table->model());

        for (int row = 0; row < generalModel->rowCount(); ++row) {
            QString addrInTable = generalModel->item(row, 0)->text();
            if (addrInTable == targetRegister1) {
                RS1 = generalModel->item(row, 1)->text();
            }
            if(addrInTable == targetRegister2){
                RS2 = generalModel->item(row, 1)->text();
            }
        }

        if((FUNCT3 == "000" || FUNCT3 == "001" || FUNCT3 == "100" || FUNCT3 == "101") && SC_n == 4){
            int RS1_V = binaryToSignedInt(RS1);
            int RS2_V = binaryToSignedInt(RS2);
            int answer = RS1_V - RS2_V ;

            if(answer > 0){
                P = 1;
            }
            else{
                P = 0;
            }

            if(answer < 0){
                N = 1;
            }
            else{
                N = 0;
            }

            if(answer == 0){
                Z = 1;
            }
            else{
                Z = 0;
            }
        }

        if((FUNCT3 == "110" || FUNCT3 == "111") && SC_n == 4){
            quint32 RS1_V = RS1.toUInt(nullptr, 2);
            quint32 RS2_V = RS2.toUInt(nullptr, 2);

            int answer = RS1_V - RS2_V ;

            if(answer > 0){
                P = 1;
            }
            else{
                P = 0;
            }

            if(answer < 0){
                N = 1;
            }
            else{
                N = 0;
            }

            if(answer == 0){
                Z = 1;
            }
            else{
                Z = 0;
            }
        }

        //beq
        if(FUNCT3 == "000" && Z == 1 && SC_n == 5){
            quint16 PC_V = PC.toUInt(nullptr, 2);
            quint16 IMM_V = IMM.toUInt(nullptr, 2);
            quint32 sum = static_cast<quint32>(PC_V) + static_cast<quint32>(IMM_V);
            quint16 answer = static_cast<quint16>(sum);

            QString binaryAnswer = QString("%1").arg(answer, 16, 2, QChar('0'));

            ui->pc_label->setText(binaryAnswer);

            SC_n = -1;
        }

        //bne
        if(FUNCT3 == "001" && Z == 0 && SC_n == 5){
            quint16 PC_V = PC.toUInt(nullptr, 2);
            quint16 IMM_V = IMM.toUInt(nullptr, 2);
            quint32 sum = static_cast<quint32>(PC_V) + static_cast<quint32>(IMM_V);
            quint16 answer = static_cast<quint16>(sum);

            QString binaryAnswer = QString("%1").arg(answer, 16, 2, QChar('0'));

            ui->pc_label->setText(binaryAnswer);

            SC_n = -1;
        }

        //blt
        if(FUNCT3 == "100" && N == 1 && SC_n == 5){
            quint16 PC_V = PC.toUInt(nullptr, 2);
            quint16 IMM_V = IMM.toUInt(nullptr, 2);
            quint32 sum = static_cast<quint32>(PC_V) + static_cast<quint32>(IMM_V);
            quint16 answer = static_cast<quint16>(sum);

            QString binaryAnswer = QString("%1").arg(answer, 16, 2, QChar('0'));

            ui->pc_label->setText(binaryAnswer);

            SC_n = -1;
        }

        //bge
        if(FUNCT3 == "101" && N == 0 && SC_n == 5){
            quint16 PC_V = PC.toUInt(nullptr, 2);
            quint16 IMM_V = IMM.toUInt(nullptr, 2);
            quint32 sum = static_cast<quint32>(PC_V) + static_cast<quint32>(IMM_V);
            quint16 answer = static_cast<quint16>(sum);

            QString binaryAnswer = QString("%1").arg(answer, 16, 2, QChar('0'));

            ui->pc_label->setText(binaryAnswer);

            SC_n = -1;
        }

        //bltu
        if(FUNCT3 == "110" && N == 1 && SC_n == 5){
            quint16 PC_V = PC.toUInt(nullptr, 2);
            quint16 IMM_V = IMM.toUInt(nullptr, 2);
            quint32 sum = static_cast<quint32>(PC_V) + static_cast<quint32>(IMM_V);
            quint16 answer = static_cast<quint16>(sum);

            QString binaryAnswer = QString("%1").arg(answer, 16, 2, QChar('0'));

            ui->pc_label->setText(binaryAnswer);

            SC_n = -1;
        }

        //bgeu
        if(FUNCT3 == "111" && N == 0 && SC_n == 5){
            quint16 PC_V = PC.toUInt(nullptr, 2);
            quint16 IMM_V = IMM.toUInt(nullptr, 2);
            quint32 sum = static_cast<quint32>(PC_V) + static_cast<quint32>(IMM_V);
            quint16 answer = static_cast<quint16>(sum);

            QString binaryAnswer = QString("%1").arg(answer, 16, 2, QChar('0'));

            ui->pc_label->setText(binaryAnswer);

            SC_n = -1;
        }
    }
}


void MainWindow::on_reset_btn_clicked()
{
    ui->pc_label->setText("0001000000000000");
    ui->funct7_label->setText("0000000");
    ui->funct3_label->setText("000");
    ui->gar_label->setText("00000");
    ui->ir_label->setText("00000000000000000000000000000000");
    ui->mar_label->setText("0000000000000000");
    ui->upcode_label->setText("0000000");
    ui->imm_label->setText("\\-");
    ui->sc_label->setText("\\- \\- \\-");
    ui->inpr_label->setText("00000000000000000000000000000000");
    ui->outr_label->setText("00000000000000000000000000000000");
}

