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
        std::string type;
        std::string opcode;
        std::string funct3;
        std::string funct7;
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
        {"add", {"R", "0110011", "000", "0000000"}},
        {"sub", {"R", "0110011", "000", "0100000"}},
        {"xor", {"R", "0110011", "100", "0000000"}},
        {"or",  {"R", "0110011", "110", "0000000"}},
        {"and", {"R", "0110011", "111", "0000000"}},
        {"sll", {"R", "0110011", "001", "0000000"}},
        {"srl", {"R", "0110011", "101", "0000000"}},
        {"sra", {"R", "0110011", "101", "0100000"}},
        {"slt", {"R", "0110011", "010", "0000000"}},
        {"sltu",{"R", "0110011", "011", "0000000"}},

        {"addi", {"I", "0010011", "000", ""}},
        {"andi", {"I", "0010011", "111", ""}},
        {"ori",  {"I", "0010011", "110", ""}},
        {"lw",   {"I", "0000011", "010", ""}},
        {"lh",   {"I", "0000011", "001", ""}},
        {"jalr", {"I", "1100111", "000", ""}},

        {"sw",   {"S", "0100011", "010", ""}},
        {"sh",   {"S", "0100011", "001", ""}},

        {"beq",  {"B", "1100011", "000", ""}},
        {"bne",  {"B", "1100011", "001", ""}},
        {"blt",  {"B", "1100011", "100", ""}},
        {"bge",  {"B", "1100011", "101", ""}},
        {"bltu", {"B", "1100011", "110", ""}},
        {"bgeu", {"B", "1100011", "111", ""}},

        {"lui",  {"U", "0110111", "", ""}},
        {"auipc",{"U", "0010111", "", ""}},

        {"jal",  {"J", "1101111", "", ""}}
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
        std::string result = assemble(line.toStdString());
        output += QString("Instruction: %1\nBinary: %2\nRegisters: x1=%3\n\n")
                      .arg(line)
                      .arg(QString::fromStdString(result))
                      .arg(registers[1]);
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
    return res;
}

std::vector<std::string> MainWindow::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    for (char c : line) {
        if (c == ' ' || c == ',') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token.push_back(c);
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

std::string MainWindow::assemble(const std::string& line) {
    std::vector<std::string> tokens = tokenize(line);
    if (tokens.empty()) return "EMPTY_LINE";

    std::string inst = tokens[0];
    if (instrMap.find(inst) == instrMap.end())
        return "UNKNOWN_INSTRUCTION";

    InstrInfo info = instrMap[inst];

    try {
        if (info.type == "R") {
            if (tokens.size() != 4) return "INVALID_R_FORMAT";
            int rd = regMap.at(tokens[1]);
            int rs1 = regMap.at(tokens[2]);
            int rs2 = regMap.at(tokens[3]);

            if (inst == "add") {
                registers[rd] = registers[rs1] + registers[rs2];
            } else if (inst == "sub") {
                registers[rd] = registers[rs1] - registers[rs2];
            } else if (inst == "xor") {
                registers[rd] = registers[rs1] ^ registers[rs2];
            } else if (inst == "or") {
                registers[rd] = registers[rs1] | registers[rs2];
            } else if (inst == "and") {
                registers[rd] = registers[rs1] & registers[rs2];
            } else if (inst == "sll") {
                registers[rd] = (unsigned)registers[rs1] << (registers[rs2] & 0x1F);
            } else if (inst == "srl") {
                registers[rd] = (unsigned)registers[rs1] >> (registers[rs2] & 0x1F);
            } else if (inst == "sra") {
                registers[rd] = registers[rs1] >> (registers[rs2] & 0x1F);
            } else if (inst == "slt") {
                registers[rd] = (registers[rs1] < registers[rs2]) ? 1 : 0;
            } else if (inst == "sltu") {
                registers[rd] = ((unsigned)registers[rs1] < (unsigned)registers[rs2]) ? 1 : 0;
            }

            return info.funct7 + to_bin(rs2, 5) + to_bin(rs1, 5) + info.funct3 + to_bin(rd, 5) + info.opcode;
        }
        else if (info.type == "I") {
            if (tokens.size() != 4) return "INVALID_I_FORMAT";
            int rd = regMap.at(tokens[1]);
            int rs1 = regMap.at(tokens[2]);
            int imm = std::stoi(tokens[3]);

            if (inst == "addi") {
                registers[rd] = registers[rs1] + imm;
            } else if (inst == "andi") {
                registers[rd] = registers[rs1] & imm;
            } else if (inst == "ori") {
                registers[rd] = registers[rs1] | imm;
            } else if (inst == "lw" || inst == "lh") {
                registers[rd] = 0;
            } else if (inst == "jalr") {
                registers[rd] = 0;
            }

            return to_bin(imm, 12) + to_bin(rs1, 5) + info.funct3 + to_bin(rd, 5) + info.opcode;
        }
        else if (info.type == "S") {
            if (tokens.size() != 4) return "INVALID_S_FORMAT";
            int rs2 = regMap.at(tokens[1]);
            int rs1 = regMap.at(tokens[2]);
            int imm = std::stoi(tokens[3]);

            std::string immBin = to_bin(imm, 12);
            std::string immHi = immBin.substr(0, 7);
            std::string immLo = immBin.substr(7, 5);

            return immHi + to_bin(rs2, 5) + to_bin(rs1, 5) + info.funct3 + immLo + info.opcode;
        }
        else if (info.type == "B") {
            if (tokens.size() != 4) return "INVALID_B_FORMAT";
            int rs1 = regMap.at(tokens[1]);
            int rs2 = regMap.at(tokens[2]);
            int imm = std::stoi(tokens[3]);

            if (inst == "beq") {
              //   (registers[rs1] == registers[rs2]);
            } else if (inst == "bne") {
                // (registers[rs1] != registers[rs2]);
            } else if (inst == "blt") {
                // (registers[rs1] < registers[rs2]);
            } else if (inst == "bge") {
                // (registers[rs1] >= registers[rs2]);
            } else if (inst == "bltu") {
                // ((unsigned)registers[rs1] < (unsigned)registers[rs2]);
            } else if (inst == "bgeu") {
                // ((unsigned)registers[rs1] >= (unsigned)registers[rs2]);
            }

            std::string immStr = to_bin(imm, 13);
            return std::string() + immStr[0] + immStr.substr(2, 6) + to_bin(rs2, 5) +
                   to_bin(rs1, 5) + info.funct3 + immStr.substr(8, 4) + immStr[1] + info.opcode;
        }
        else if (info.type == "U") {
            if (tokens.size() != 3) return "INVALID_U_FORMAT";
            int rd = regMap.at(tokens[1]);
            int imm = std::stoi(tokens[2]);

            if (inst == "lui") {
                registers[rd] = imm << 12;
            } else if (inst == "auipc") {
                registers[rd] = 0 + (imm << 12);
            }

            return to_bin(imm, 20) + to_bin(rd, 5) + info.opcode;
        }
        else if (info.type == "J") {
            if (tokens.size() != 3) return "INVALID_J_FORMAT";
            int rd = regMap.at(tokens[1]);
            int imm = std::stoi(tokens[2]);

            if (inst == "jal") {
                registers[rd] = 0 + 4;
            }

            std::string immStr = to_bin(imm, 21);
            return std::string() + immStr[0] + immStr.substr(10, 10) + immStr[9] +
                   immStr.substr(1, 8) + to_bin(rd, 5) + info.opcode;
        }
    }
    catch (const std::exception& e) {
        return "ERROR: " + std::string(e.what());
    }

    return "UNSUPPORTED";
}
