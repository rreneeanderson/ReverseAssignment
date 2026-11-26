
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Instructions {
    string inst;
    int code;
    int functionCode;
};

struct RegTable {
    string reg;
    int address;
};

string registerName(int number, RegTable registers[], int count) {
    for (int i = 0; i < count; i++) {
        if (registers[i].address == number)
            return registers[i].reg;
    }
    return "unknown";
}

int main(int argc, char* argv[]) {
    Instructions instr[5] = {
        {"add", 0, 32},
        {"addi", 8, 0},
        {"sub", 0, 34},
        {"lw", 35, 0},
        {"sw", 43, 0}
    };

    RegTable registers[6] = {
        {"$0", 0},
        {"$t0", 8},
        {"$t1", 9},
        {"$t2", 10},
        {"$s0", 16},
        {"$s1", 17}
    };

    string filename = (argc > 1) ? argv[1] : "test.txt";
    ifstream file(filename);

    if (!file) {
        cout << "Could not open " << filename << endl;
        return 1;
    }

    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        // Convert hexadecimal into a number
        unsigned int machineCode;

        try {
            size_t used = 0;
            unsigned long value = stoul(line, &used, 16);

            if (used != line.size() || value > 0xFFFFFFFFUL)
                throw 1;

            machineCode = static_cast<unsigned int>(value);
        }
        catch (...) {
            cout << "Invalid instruction: " << line << endl;
            continue;
        }

        // Get the parts of the instruction
        int opcode = (machineCode >> 26) & 63;
        int rs = (machineCode >> 21) & 31;
        int rt = (machineCode >> 16) & 31;
        int rd = (machineCode >> 11) & 31;
        int funct = machineCode & 63;

        int immediate = machineCode & 65535;
        if (immediate >= 32768)
            immediate -= 65536;

        // Find the instruction name
        string name = "";

        for (int i = 0; i < 5; i++) {
            if (opcode == instr[i].code &&
                (opcode != 0 || funct == instr[i].functionCode)) {
                name = instr[i].inst;
                break;
            }
        }

        string rS = registerName(rs, registers, 6);
        string rT = registerName(rt, registers, 6);
        string rD = registerName(rd, registers, 6);

        // Print the assembly instruction
        if (name == "add" || name == "sub") {
            cout << name << " " << rD << ", "
                 << rS << ", " << rT << endl;
        }
        else if (name == "addi") {
            cout << name << " " << rT << ", "
                 << rS << ", " << immediate << endl;
        }
        else if (name == "lw" || name == "sw") {
            cout << name << " " << rT << ", "
                 << immediate << "(" << rS << ")" << endl;
        }
        else {
            cout << "Unknown instruction" << endl;
        }
    }

    file.close();
    return 0;
}
