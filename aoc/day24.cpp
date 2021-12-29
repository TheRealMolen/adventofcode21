#include "pch.h"
#include "harness.h"
#include <intrin.h>

enum class AluClass { Add, Div, Eql, Inp, Mul, Mod };


struct AluOp
{
    uint16_t instr : 3;
    uint16_t reg_a : 2;
    uint16_t isreg_b : 1;
    uint16_t reg_b : 2;
    uint16_t imm_b : 8;    // offset by 100

    int getImm() const { return int(imm_b) - 100; }
};
static_assert(sizeof AluOp == 2);

using AluProgram = vector<AluOp>;


AluProgram parseAluProgram(const stringlist& input)
{
    using enum AluClass;
    AluProgram prog;
    prog.reserve(input.size());

    for (auto& line : input)
    {
        AluOp op = {0};

        switch(line[1])
        {
        case 'd':   op.instr = uint16_t(Add); break;
        case 'i':   op.instr = uint16_t(Div); break;
        case 'q':   op.instr = uint16_t(Eql); break;
        case 'n':   op.instr = uint16_t(Inp); break;
        case 'u':   op.instr = uint16_t(Mul); break;
        case 'o':   op.instr = uint16_t(Mod); break;
        default:    throw "nah";
        }

        uint16_t rega = line[4] - 'w';
        _ASSERT(rega < 4);
        op.reg_a = rega;

        if (op.instr != uint16_t(Inp))
        {
            char c6 = line[6];
            if (c6 >= 'w' && c6 <= 'z')
            {
                op.isreg_b = 1;
                uint16_t regb = c6 - 'w';
                _ASSERT(regb < 4);
                op.reg_b = regb;
            }
            else
            {
                op.isreg_b = 0;
                int imm = atoi(line.c_str() + 6);
                _ASSERT(imm > -100 && imm < 100);
                op.imm_b = uint16_t(imm + 100);
            }
        }

        prog.push_back(op);
    }

    return prog;
}

int g_bp = -1;

int64_t runAluProg(const AluProgram& prog, const char* input)
{
    using enum AluClass;
    const char* nextInp = input;
    int64_t regs[4] = {0,0,0,0};

    auto itBreak = end(prog);
    if (g_bp >= 0)
        itBreak = begin(prog) + g_bp;

    int pc = 1;
    for (auto itOp = begin(prog); itOp != end(prog); ++itOp, ++pc)
    {
        AluOp op = *itOp;
        if (itOp == itBreak)
        {
            cout << "BREAK at pc=" << pc << endl;
            __debugbreak();
        }

        switch(AluClass(op.instr))
        {
        case Add:
            if (op.isreg_b)
                regs[op.reg_a] += regs[op.reg_b];
            else
                regs[op.reg_a] += op.getImm();
            break;

        case Mul:
            if (op.isreg_b)
                regs[op.reg_a] *= regs[op.reg_b];
            else
                regs[op.reg_a] *= op.getImm();
            break;

        case Div:
            if (op.isreg_b)
                regs[op.reg_a] /= regs[op.reg_b];
            else
                regs[op.reg_a] /= op.getImm();
            break;

        case Mod:
            if (op.isreg_b)
                regs[op.reg_a] %= regs[op.reg_b];
            else
                regs[op.reg_a] %= op.getImm();
            break;

        case Eql:
            if (op.isreg_b)
                regs[op.reg_a] = (regs[op.reg_a] == regs[op.reg_b]) ? 1 : 0;
            else
                regs[op.reg_a] = (regs[op.reg_a] == op.getImm()) ? 1 : 0;
            break;

        case Inp:
            _ASSERT(nextInp);
            _ASSERT(*nextInp);
            regs[op.reg_a] = (*nextInp) - '0';
            ++nextInp;
            break;
        }
    }

    return regs[3]; // z
}


void testMonad(const stringlist& source, const char* inp)
{
    _ASSERT(strlen(inp) >= 14);
    int64_t in[14];
    for (int i=0; i<14; ++i)
        in[i] = inp[i] - '0';

    AluProgram monad = parseAluProgram(source);

#if 0
#define TEST_PART(n, pc)        cout << "testing part " #n ":" << endl;     \
                                AluProgram monPart##n(begin(monad), begin(monad) + pc);     \
                                test(runAluProg(monPart##n, inp), z_part##n)
#else
#define TEST_PART(n, pc)        do{}while(0)
#endif

    int64_t z_part1 = (((in[0] + 7) * 26) + in[1] + 4);
    TEST_PART(1, 40);

    int64_t in23valid = (in[3] != (in[2] + 4));
    int64_t y_part2 = in23valid ? 26 : 1;
    [[maybe_unused]] int64_t z_part2 = z_part1 * y_part2;
    TEST_PART(2, 50);

    int64_t z_part3 = (z_part1 * (in23valid ? 26 : 1)) + (in[3] + 1) * in23valid;
    TEST_PART(3, 74);
    int64_t z_part4 = (z_part3 * 26) + (in[4] + 5);

    int64_t z_part5 = (z_part4 * 26) + in[5] + 14;
    TEST_PART(5, 110);

    int64_t in67valid = (in[6] + 3) != in[7];
    int64_t z_part6 = z_part5 * (in67valid ? 26 : 1);
    TEST_PART(6, 140);

    int64_t z_part7 = z_part6 + ((in[7] + 10) * in67valid);
    TEST_PART(7, 146);

    int64_t in8valid = ((z_part7 % 26) - 9) != in[8];
    int64_t z_part8 = ((z_part7 / 26) * (in8valid ? 26 : 1)) + ((in[8] + 5) * in8valid);
    TEST_PART(8, 164);

    int64_t in910valid = (in[9] - 8) != in[10];
    int64_t z_part9 = (z_part8 * (in910valid ? 26 : 1)) + ((in[10] + 6) * in910valid);
    TEST_PART(9, 200);

    int64_t in11valid = ((z_part9 % 26) - 7) != in[11];
    int64_t z_part10 = (z_part9 / 26) * (in11valid ? 26 : 1) + ((in[11] + 8) * in11valid);
    TEST_PART(10, 218);

    int64_t in12valid = ((z_part10 % 26) - 10) != in[12];
    int64_t z_part11 = ((z_part10 / 26) * (in12valid ? 26 : 1)) + ((in[12] + 4) * in12valid);

    int64_t in13valid = (z_part11 % 26) != in[13];
    int64_t z_final = ((z_part11 / 26) * (in13valid ? 26 : 1)) + ((in[13] + 6) * in13valid);
    test(z_final, runAluProg(monad, inp));
}


int day24(const stringlist& input)
{
    for (auto& line : input)
    {
        (void)line;
    }

    return -1;
}

int day24_2(const stringlist& input)
{
    for (auto& line : input)
    {
        (void)line;
    }

    return -1;
}


void run_day24()
{
    string cmp3x =
        R"(inp z
inp x
mul z 3
eql z x)";
    string mkPi =
R"(add x 4
add y 10
add w y
mul w 10
mul x w
add z x
add z -86
mul z y
add z 2)";

    auto progCmp3x = parseAluProgram(READ(cmp3x));
    auto progMkPi = parseAluProgram(READ(mkPi));

    test(1, runAluProg(progCmp3x, "26"));
    test(0, runAluProg(progCmp3x, "29"));
    test(3142, runAluProg(progMkPi, ""));
    testMonad(LOAD(24), "12351678912345");
    testMonad(LOAD(24), "99988877766655");
    testMonad(LOAD(24), "11111111111111");
    //gogogo(day24(LOAD(24)));

    //test(-100, day24_2(READ(sample)));
    //gogogo(day24_2(LOAD(24)));
}