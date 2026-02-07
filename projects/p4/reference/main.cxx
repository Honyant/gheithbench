#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <vector>

#include "slice.h"

using namespace std;
typedef struct  // replicate std::optional
{
    bool has_value;
    uint64_t value;
} optional_uint64_t;

typedef struct  // replicate std::optional
{
    bool has_value;
    Slice value;
} optional_Slice;

vector<string> instructions;
unordered_map<uint64_t, vector<string>> function_instructions;
// given an int, it will return a vector of strings
// that are the instructions for that function

vector<uint64_t> function_start_list;
set<string> variable_names;

uint64_t currFunction = 0;
int in_function_depth = 0;
uint64_t double_and_counter = 0;
vector<int> if_counter;
vector<int> while_counter;
string lastReadId = "";
bool literal = true;
int64_t last_function_call = -1;
string last_iden = "";
uint64_t blr_index = 0;
string prevConsume = "";
string prevprevConsume = "";

void printff(string str) {
    if (in_function_depth > 0) {
        function_instructions[currFunction].push_back(str);
    } else {
        instructions.push_back(str);
    }
}
string while_counter_string() {
    string ret = "_";
    for (auto i : while_counter) {
        ret += to_string(i);
        if ((uint64_t)i != while_counter.size() - 1) {
            ret += "_";
        }
    }
    return ret;
}

string if_counter_string() {
    string ret = "_";
    for (auto i : if_counter) {
        ret += to_string(i);
        if ((uint64_t)i != if_counter.size() - 1) {
            ret += "_";
        }
    }
    return ret;
}

void initVars() {
    for (auto i : variable_names) {
        printf("%s: \n", i.c_str());
        printf("   .quad 0\n");
    }
    if (variable_names.find("var_argc") == variable_names.end()) {
        printf("var_argc:\n");
        printf("   .quad 0\n");
    }
    printf("fmtvarString:\n");
    printf("   .string \"%%ld\\n\"\n");
    printf(".align 16\n");
}

void loadVar(string var, int r1) {
    printff("   adrp x" + to_string(r1) + ", " + var + " //load var\n");
    printff("   add x" + to_string(r1) + ", x" + to_string(r1) +
            ", :lo12:" + var + "\n");
    printff("   ldr x" + to_string(r1) + ", [x" + to_string(r1) + "]\n");
}

void setVar(string var, int r1, int r2) {
    printff("   adrp x" + to_string(r2) + ", " + var + " //set var\n");
    printff("   add x" + to_string(r2) + ", x" + to_string(r2) +
            ", :lo12:" + var + "\n");
    printff("   str x" + to_string(r1) + ", [x" + to_string(r2) + "]\n");
}

void loadAdr(string var, int r1) {
    printff("   adrp x" + to_string(r1) + ", " + var + " //load adr\n");
    printff("   add x" + to_string(r1) + ", x" + to_string(r1) +
            ", :lo12:" + var + "\n");
}

void storeVar(string var, int varReg, int r1) {
    printff("   adrp x" + to_string(varReg) + ", " + var + " //store var\n");
    printff("   add x" + to_string(varReg) + ", x" + to_string(varReg) +
            ", :lo12:" + var + "\n");
    printff("   str x" + to_string(r1) + ", [x" + to_string(varReg) + "]\n");
}

void saveRegToStack(int reg) {
    printff("   str x" + to_string(reg) +
            ", [sp, #-16]! //save reg to stack\n");
}
void loadRegFromStack(int reg) {
    printff("   ldr x" + to_string(reg) +
            ", [sp], #16 //load reg from stack\n");
}
void storeFPLR() {
    printff("   stp x29, x30, [sp, #-16]! //store fp and lr\n");
    printff("   mov x29, sp\n");
}
void restoreFPLR() {
    printff("   mov sp, x29 //restore fp and lr\n");
    printff("   ldp x29, x30, [sp], #16\n");
}

void moveLiteral(int reg, uint64_t val) {
    printff("   movz x" + to_string(reg) + ", #" + to_string(val & 0xFFFF) +
            ", lsl #0\n");
    printff("   movk x" + to_string(reg) + ", #" +
            to_string((val >> 16) & 0xFFFF) + ", lsl #16\n");
    printff("   movk x" + to_string(reg) + ", #" +
            to_string((val >> 32) & 0xFFFF) + ", lsl #32\n");
    printff("   movk x" + to_string(reg) + ", #" +
            to_string((val >> 48) & 0xFFFF) + ", lsl #48\n");
}

class Compiler {
    char const *const program;
    char const *current;
    uint64_t *it;
    uint64_t *id;

    void fail() {
        printf("failed at offset %ld\n", (size_t)(current - program));
        printf("%s\n", current);
        exit(1);
    }

    void end_or_fail() {
        while (isspace(*current)) {
            current += 1;
        }
        if (*current != 0) fail();
    }

    void consume_or_fail(const char *str) {
        if (!consume(str)) {
            fail();
        }
    }

    void skip() {
        while (isspace(*current)) {
            current += 1;
        }
    }

    bool consume(const char *str) {
        skip();

        size_t i = 0;
        while (true) {
            char const expected = str[i];
            char const found = current[i];
            if (expected == 0) {
                /* survived to the end of the expected string */
                current += i;
                prevConsume = str;
                prevprevConsume = prevConsume;
                return true;
            }
            if (expected != found) {
                return false;
            }
            // assertion: found != 0
            i += 1;
        }
    }

    optional_uint64_t consume_literal_Test() {
        skip();

        if (isdigit(*current)) {
            uint64_t v = 0;
            do {
                v = 10 * v + ((*current) - '0');
                current += 1;
            } while (isdigit(*current));
            return (optional_uint64_t){true, v};
        } else {
            return (optional_uint64_t){false, 0};
        }
    }

    uint64_t e0Test(bool effects) {// in this e{num}Test chain, we test if the expression is all literals, so we can optimize it. 
    // manually checknig for literals wouldn't work because of the recursive nature of the parser
        optional_uint64_t v = consume_literal();
        if (v.has_value) {
            return v.value;
        }
        if (consume("(")) {
            uint64_t v = expressionTest(effects);
            if (!literal) {
                return v;
            }
            consume(")");
            return v;
        } else {
            literal = false;
            return 0;
        }
    }

    uint64_t e1Test(bool effects) {
        uint64_t v = e0Test(effects);
        if (!literal) {
            return v;
        }
        while (true) {
            if (consume("(")) {
                literal = false;
                return 0;
            } else {
                return v;
            }
        }
    }
    // ++ -- unary+ unary- ... (Right)
    uint64_t e2Test(bool effects) { return e1Test(effects); }

    // * / % (Left)
    uint64_t e3Test(bool effects) {
        uint64_t v = e2Test(effects);
        while (true) {
            if (!literal) {
                return v;
            }
            if (consume("*")) {
                v = v * e2Test(effects);
            } else if (consume("/")) {
                uint64_t right = e2Test(effects);
                v = (right == 0) ? 0 : v / right;
            } else if (consume("%")) {
                uint64_t right = e2Test(effects);
                v = (right == 0) ? 0 : v % right;
            } else {
                return v;
            }
        }
    }

    // (Left) + -
    uint64_t e4Test(bool effects) {
        uint64_t v = e3Test(effects);
        while (true) {
            if (!literal) {
                return v;
            }
            if (consume("+")) {
                v = v + e3Test(effects);
            } else if (consume("-")) {
                v = v - e3Test(effects);
            } else {
                return v;
            }
        }
    }

    // << >>
    uint64_t e5Test(bool effects) { return e4Test(effects); }

    // < <= > >=
    uint64_t e6Test(bool effects) {
        uint64_t v = e5Test(effects);

        while (true) {
            if (!literal) {
                return v;
            }
            if (consume("<=")) {
                v = v <= e5Test(effects);
            } else if (consume(">=")) {
                v = v >= e5Test(effects);
            } else if (consume("<")) {
                v = v < e5Test(effects);
            } else if (consume(">")) {
                v = v > e5Test(effects);
            } else {
                return v;
            }
        }
    }

    // == !=
    uint64_t e7Test(bool effects) {
        uint64_t v = e6Test(effects);

        while (true)  // address short circuiting
        {
            if (!literal) {
                return v;
            }
            if (consume("==")) {
                v = v == e6Test(effects);
            } else if (consume("!=")) {
                v = v != e6Test(effects);
            } else {
                return v;
            }
        }
    }

    // (left) &
    uint64_t e8Test(bool effects) {
        uint64_t v = e7Test(effects);
        while (true) {
            if (!literal) {
                return v;
            }
            if (consume("&")) {
                if (consume("&")) {
                    current -= 2;
                    return v;
                } else {
                    v = v & e7Test(effects);
                }
            } else {
                return v;
            }
        }
    }

    // ^
    uint64_t e9Test(bool effects) { return e8Test(effects); }

    // |
    uint64_t e10Test(bool effects) { return e9Test(effects); }

    // &&
    uint64_t e11Test(bool effects) {
        uint64_t v = e10Test(effects);
        while (true) {
            if (!literal) {
                return v;
            }
            if (consume("&&")) {
                if (v) {
                    v = v && e10Test(effects);
                } else {
                    v = 0;
                    e10Test(false);
                }
            } else {
                return v;
            }
        }
    }

    // ||
    uint64_t e12Test(bool effects) {
        uint64_t v = e11Test(effects);

        while (true) {
            if (!literal) {
                return v;
            }
            if (consume("||")) {
                if (v) {
                    v = 1;
                    e11Test(false);
                } else {
                    v = v || e11Test(effects);
                }
            } else {
                return v;
            }
        }
    }

    // (right with special treatment for middle expression) ?:
    uint64_t e13Test(bool effects) { return e12Test(effects); }

    // = += -= ...
    uint64_t e14Test(bool effects) { return e13Test(effects); }

    // ,
    uint64_t e15Test(bool effects) {
        uint64_t v = e14Test(effects);
        while (true) {
            if (!literal) {
                return v;
            }
            if (consume(",")) {
                v = e14Test(effects);
            } else {
                return v;
            }
        }
    }

    uint64_t expressionTest(bool effects) { return e15Test(effects); }

    optional_Slice consume_identifier() {
        skip();
        if (isalpha(*current)) {
            char const *start = current;
            do {
                current += 1;
            } while (isalnum(*current));
            Slice a = {start, (size_t)(current - start)};
            return (optional_Slice){true, a};
        } else {
            Slice a = {"", (size_t)0};
            return (optional_Slice){false, a};
        }
    }

    optional_uint64_t consume_literal() {
        skip();
        if (isdigit(*current)) {
            uint64_t v = 0;
            do {
                v = 10 * v + ((*current) - '0');
                current += 1;
            } while (isdigit(*current));
            return (optional_uint64_t){true, v};
        } else {
            return (optional_uint64_t){false, 0};
        }
    }

    u_int64_t get_current_offset() { return (u_int64_t)(current - program); }

    void e0(bool effects) {
        if (consume("fun")) {
            if (isalnum(*current)) {  // consider removing?
                current -= 3;
            } else {
                // try to get function TODO: cehck edge cases
                // that begin with fun
                if (consume("{")) {
                    in_function_depth++;
                    uint64_t index = get_current_offset();
                    function_start_list.push_back(index);

                    uint64_t oldIndex = currFunction;
                    currFunction = index;
                    printff("fun_" + to_string(index) + ":\n");
                    storeFPLR();
                    last_function_call = -1;
                    while (!consume("}")) {
                        statement(true);
                    }
                    bool tail_recursion = false;
                    // move currPtr backwards and see if we can get to the index of the last function call when only seeing } or whitespace
                    char* currPtr = (char*)current;
                    while(currPtr > program && (isspace(*currPtr) || *currPtr == '}')) {
                        currPtr--;
                        if (currPtr - program == last_function_call) {
                            // we have tail recursion
                            tail_recursion = true;
                            break;
                        }
                    }
                    if (tail_recursion && blr_index != 0) {
                        auto& fInstr = function_instructions[currFunction];

                        fInstr.erase(fInstr.begin() + blr_index - 2, fInstr.begin() + blr_index - 1);

                        blr_index -= 1;
                        fInstr.erase(fInstr.begin() + blr_index, fInstr.begin() + blr_index + 1);
                        fInstr.insert(fInstr.begin() + blr_index, "   mov sp, x29 //restore fp and lr\n");
                        fInstr.insert(fInstr.begin() + blr_index + 1, "   ldp x29, x30, [sp], #16\n");
                        fInstr.insert(fInstr.begin() + blr_index + 2, "   br x1\n");
                    }

                    currFunction = index;
                    printff("   mov x0, #0\n");
                    printff("fun_" + to_string(index) + "_end:\n");
                    restoreFPLR();
                    printff("   ret\n\n");
                    in_function_depth--;
                    currFunction = oldIndex;
                    loadAdr("fun_" + to_string(index), 0);
                    if (effects) {
                        return;
                    }
                    return;
                } else {
                    in_function_depth++;
                    uint64_t index = get_current_offset();
                    function_start_list.push_back(index);
                    uint64_t oldIndex = currFunction;
                    currFunction = index;
                    printff("fun_" + to_string(index) + ":\n");
                    storeFPLR();
                    bool success = statement(true);
                    currFunction = index;
                    printff("   mov x0, #0\n");
                    printff("fun_" + to_string(index) + "_end:\n");
                    restoreFPLR();
                    printff("   ret\n\n");
                    in_function_depth--;
                    currFunction = oldIndex;
                    loadAdr("fun_" + to_string(index), 0);
                    if (effects) return;
                    if (!success) fail();
                    return;
                }
            }
        }

        optional_Slice id = consume_identifier();
        if (id.has_value) {  // given an id, it could be a normal symbol, a
                             // function symbol, or an it, and we need to tell
                             // the difference
            last_iden = id.value.getString();
            if (!(id.value == "it" && in_function_depth > 0)) {
                loadVar("var_" + id.value.getString(), 0);
                if (!(id.value == "argc"))
                    variable_names.insert("var_" + id.value.getString());
            } else {
                printff("   mov x0, x6\n");
            }
            return;
        }
        optional_uint64_t v = consume_literal();
        if (v.has_value) {
            moveLiteral(0, v.value);
            return;
        }
        if (consume("(")) {
            expression(effects);
            consume(")");
            return;
        } else {
            fail();
            return;  // this should never be reached bcs of fail() but im
                     // putting it here to make the compiler happy
        }
    }

    void e1(bool effects) {
        e0(effects);
        while (true) {
            bool tailcallFlag = prevprevConsume == "return";

            if (consume("(")) {  // call the function
                saveRegToStack(0);
                expression(effects);
                loadRegFromStack(1);
                saveRegToStack(6);
                printff("   mov x6, x0\n");
                printff("   blr x1\n");
                blr_index = tailcallFlag?function_instructions[currFunction].size()-1:0;
                loadRegFromStack(6);
                consume(")");
                last_function_call = get_current_offset();
            } else {
                return;
            }
        }
    }

    // ++ -- unary+ unary- ... (Right)
    void e2(bool effects) { e1(effects); }

    // * / % (Left)
    void e3(bool effects) {
        e2(effects);
        while (true) {
            if (consume("*")) {
                saveRegToStack(0);
                e2(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   mul x0, x0, x1\n");
            } else if (consume("/")) {  // TODO: consider saving 0,1,2 to stack
                saveRegToStack(0);
                e2(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   udiv x0, x0, x1\n");
            } else if (consume("%")) {
                saveRegToStack(0);
                e2(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   udiv x2, x0, x1\n");
                printff("   msub x2, x2, x1, x0\n");
                printff("   cmp x1, #0\n");
                printff("   csel x0, xzr, x2, eq\n");
            } else {
                return;
            }
        }
    }

    // (Left) + -
    void e4(bool effects) {
        e3(effects);

        while (true) {
            if (consume("+")) {
                saveRegToStack(0);
                e3(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   add x0, x0, x1\n");
            } else if (consume("-")) {
                saveRegToStack(0);
                e3(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   sub x0, x0, x1\n");
            } else {
                return;
            }
        }
    }

    // << >>
    void e5(bool effects) { e4(effects); }

    // < <= > >=
    void e6(bool effects) {
        e5(effects);

        while (true) {
            if (consume("<=")) {
                saveRegToStack(0);
                e5(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   cmp x0, x1\n");
                printff("   cset w0, ls\n");
            } else if (consume(">=")) {
                saveRegToStack(0);
                e5(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   cmp x0, x1\n");
                printff("   cset w0, hs\n");
            } else if (consume("<")) {
                saveRegToStack(0);
                e5(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   cmp x0, x1\n");
                printff("   cset w0, lo\n");
            } else if (consume(">")) {
                saveRegToStack(0);
                e5(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   cmp x0, x1\n");
                printff("   cset w0, hi\n");
            } else {
                return;
            }
        }
    }

    // == !=
    void e7(bool effects) {
        e6(effects);
        while (true)  // address short circuiting
        {
            if (consume("==")) {
                saveRegToStack(0);
                e6(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   cmp x0, x1\n");
                printff("   cset x0, eq\n");
            } else if (consume("!=")) {
                saveRegToStack(0);
                e6(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   cmp x0, x1\n");
                printff("   cset x0, ne\n");
            } else {
                return;
            }
        }
    }

    // (left) &
    void e8(bool effects) {
        e7(effects);
        while (true) {
            if (consume("&")) {
                if (consume("&")) {
                    current -= 2;
                    return;
                } else {
                    saveRegToStack(0);
                    e7(effects);
                    printff("   mov x1, x0\n");
                    loadRegFromStack(0);
                    printff("   and x0, x0, x1\n");
                }
            } else {
                return;
            }
        }
    }

    // ^
    void e9(bool effects) { e8(effects); }

    // |
    void e10(bool effects) { e9(effects); }

    // &&
    void e11(bool effects) {
        e10(effects);
        while (true) {
            if (consume("&&")) {  // TODO do short circuiting when someone
                                  // figures out how to make us in pain
                // name the label based on function_start_list
                double_and_counter++;
                uint64_t labelnum = double_and_counter;
                saveRegToStack(0);
                printff("   cbz x0, .LDOUBLEDAND" + to_string(labelnum) + "\n");
                e10(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   cmp x0, #0\n");
                printff("   cset x2, ne\n");
                printff("   cmp x1, #0\n");
                printff("   cset x3, ne\n");
                printff("   and x0, x2, x3\n");
                printff("   b .LDOUBLEDANDSKIP" + to_string(labelnum) + "\n");
                printff(".LDOUBLEDAND" + to_string(labelnum) + ":\n");
                loadRegFromStack(0);
                printff("   mov x0, #0\n");
                printff(".LDOUBLEDANDSKIP" + to_string(labelnum) + ":\n");
            } else {
                return;
            }
        }
    }

    // ||
    void e12(bool effects) {
        e11(effects);
        while (true) {
            if (consume("||")) {
                // orr     w8, w1, w0
                // cmp     w8, #0                  // =0
                // cset    w0, ne
                uint64_t labelnum = get_current_offset();
                saveRegToStack(0);
                printff("   cbnz x0, .LDOUBLEOR" + to_string(labelnum) + "\n");
                e11(effects);
                printff("   mov x1, x0\n");
                loadRegFromStack(0);
                printff("   orr x2, x1, x0\n");
                printff("   cmp x2, #0\n");
                printff("   cset x0, ne\n");
                printff("   b .LDOUBLEORSKIP" + to_string(labelnum) + "\n");
                printff(".LDOUBLEOR" + to_string(labelnum) + ":\n");
                loadRegFromStack(0);
                printff("   mov x0, #1\n");
                printff(".LDOUBLEORSKIP" + to_string(labelnum) + ":\n");
            } else {
                return;
            }
        }
    }

    // (right with special treatment for middle expression) ?:
    void e13(bool effects) { e12(effects); }

    // = += -= ...
    void e14(bool effects) { e13(effects); }

    // ,
    void e15(bool effects) {
        e14(effects);
        while (true) {
            if (consume(",")) {
                e14(effects);
            } else {
                return;
            }
        }
    }

    void expression(bool effects) {
        literal = true;
        const char *currPtr = current;
        uint64_t value = expressionTest(effects);
        if (literal) {
            moveLiteral(0, value);
        } else {
            current = currPtr;
            e15(effects);
        }
    }

    void printLine() {
        char *curr = (char *)current;
        while (*curr != '\n') {
            printf("%c", *curr);
            curr++;
        }
        printf("\n");
    }

    bool statement(bool effects) {
        // printLine();
        if (consume("print")) {
            expression(effects);
            saveRegToStack(6);
            saveRegToStack(0);
            saveRegToStack(1);
            printff("   mov x1, x0\n");
            loadAdr("fmtvarString", 0);
            printff("   bl printf\n");
            printff("   mov x0, x1\n");
            loadRegFromStack(1);
            loadRegFromStack(0);
            loadRegFromStack(6);
            return true;
        } else if (consume("if")) {
            if_counter[if_counter.size() - 1] += 1;
            if_counter.push_back(1);
            string label = if_counter_string();
            expression(effects);
            printff("   cbz x0, .LELSE" + label + "\n");
            if (consume("{")) {
                while (!consume("}")) {
                    statement(effects);
                }
            } else {
                statement(effects);
            }
            printff("   b .LIFEND" + label + "\n");
            printff(".LELSE" + label + ":\n");
            if (consume("else")) {
                if (consume("{")) {
                    while (!consume("}")) {
                        statement(effects);
                    }
                } else {
                    statement(effects);
                }
            }
            printff(".LIFEND" + label + ":\n");
            if_counter.pop_back();
            return true;
        } else if (consume("while")) {
            while_counter[while_counter.size() - 1] += 1;
            string label = while_counter_string();
            while_counter.push_back(1);
            printff(".LWHILEEXPR" + label + ":\n");
            expression(effects);
            printff("   b .LWHILEEND" + label + "\n");
            printff(".LWHILE" + label + ":\n");

            if (consume("{")) {
                while (!consume("}")) {
                    statement(effects);
                }
            } else {
                statement(effects);
            }
            printff("   b .LWHILEEXPR" + label + "\n");
            printff(".LWHILEEND" + label + ":\n");
            printff("   cbnz x0, .LWHILE" + label + "\n");
            while_counter.pop_back();
            return true;
        } else if (consume("return")) {
            expression(effects);
            printff(
                "   b fun_" +
                to_string(function_start_list[function_start_list.size() - 1]) +
                "_end\n");
            return true;
        }
        optional_Slice id = consume_identifier();
        if (id.has_value) {
            consume("=");
            expression(effects);
            variable_names.insert("var_" + id.value.getString());
            if (id.value == "it" && in_function_depth > 0) {
                printff("   mov x6, x0\n");
            } else {
                storeVar("var_" + id.value.getString(), 1, 0);
            }
            return true;
        }
        return false;
    }

    void statements(bool effects) {
        while (statement(effects))
            ;
    }

   public:
    Compiler(char const *prog) : program(prog), current(prog) {}

    inline Compiler(char const *const program, char const *current,
                    uint64_t *it, uint64_t *id)
        : program{program}, current{current}, it{it}, id{id} {}

    void run() {
        statements(true);
        end_or_fail();
    }
};

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s <fun file name>\n", argv[0]);
        exit(-1);
    }

    // open the file
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    // determine its size (std::filesystem::map_get_size?)
    struct stat file_stats;
    int rc = fstat(fd, &file_stats);
    if (rc != 0) {
        perror("fstat");
        exit(1);
    }

    // map the file in my address space
    char *prog =
        (char *)mmap(0, file_stats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (prog == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    while_counter.push_back(1);
    if_counter.push_back(1);
    Compiler compiler = Compiler(prog, prog, NULL, NULL);
    setVar("var_argc", 0, 1);
    storeFPLR();
    compiler.run();
    restoreFPLR();
    printf(".data\n");
    initVars();
    printf(".text\n");
    printf(".global main\n");
    printf("main:\n");
    printf("   mov x6, #0\n");
    for (size_t i = 0; i < instructions.size(); i++) {
        printf("%s", instructions[i].c_str());
    }
    printf("   mov x0, #0\n");
    printf("   ret\n");
    // iterate thru map and print out the all the elements in the vector value
    for (auto i : function_instructions) {
        for (auto j : i.second) {
            printf("%s", j.c_str());
        }
    }
    return 0;
}