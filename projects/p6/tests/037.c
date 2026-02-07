#include <stdio.h>

#include "src/go.h"

/**
 *  This test just tests symbols
 *  and prints a penguin
 *
 *       .___.
 *      /     \
 *     | O _ O |
 *     /  \_/  \ 
 *   .' /     \ `.
 *  / _|       |_ \
 * (_/ |       | \_)
 *     \       /
 *    __\_>-<_/__
 *    ~;/     \;~
 *
 *
*/
char penguin [] = "      .___.\n     /     \\\n    | O _ O |\n    /  \\_/  \\ \n  .' /     \\ `.\n / _|       |_ \\\n(_/ |       | \\_)\n    \\       /\n   __\\_>-<_/__\n   ~;/     \\;~                                                                                  ";
int i = 0;
Value print_penguin() {
    if (i < 200) {
        send(me(), asChar(penguin[i++]));
        again();
    }
    return asChar('@');
}


int main() {
    Channel *out = go(print_penguin);

    while (1) {
        char c = receive(out).asChar;
        if (c == '@')
            break;
        printf("%c", c);
    }
    return 0;
}

//sorry !
void yield() {}
void wrapper() {}
void magic() {}
void context_switch() {}
void initiator() {}
void init() {}