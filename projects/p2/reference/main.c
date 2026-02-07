#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hashmap.h"
#include "slice.h"

typedef struct // replicate std::optional
{
    bool has_value;
    uint64_t value;
} optional_uint64_t;

typedef struct // replicate std::optional
{
    bool has_value;
    Slice value;
} optional_Slice;

typedef struct
{
    char *program;
    char *current;
    uint64_t *it;
    uint64_t *id;
    
} Interpreter;

uint64_t return_register;
size_t function_counter = 1;
Hashmap symbol_table;
Hashmap function_table;
Slice *curr_slice;
bool return_flag = false;
char *function_list[1 << 18];
char *function_end_list[1 << 18];

bool consume(Interpreter *interpreter, const char *str);
uint64_t expression(Interpreter *interpreter, bool effects);
void run(Interpreter *interpreter);
bool statement(Interpreter *interpreter, bool effects);

void fail(Interpreter *interpreter)
{
    printf("failed at offset %ld\n",
           (size_t)(interpreter->current - interpreter->program));
    printf("%s\n", interpreter->current);
    map_destroy(&symbol_table);
    map_destroy(&function_table);
    if (curr_slice)
        free(curr_slice);
    if (!interpreter->it)
        exit(1);
}

void end_or_fail(Interpreter *interpreter)
{
    while (isspace(*interpreter->current))
    {
        interpreter->current += 1;
    }
    if (interpreter->id)
    {
        while (isspace(*interpreter->current))
        {
            interpreter->current += 1;
        }
        if (interpreter->current < (function_end_list[*interpreter->id] - 1) &&
            !return_flag)
            fail(interpreter); // TODO: fix this
    }
    else
    {
        if (*interpreter->current != 0)
            fail(interpreter);
    }
}

void consume_or_fail(Interpreter *interpreter, const char *str)
{
    if (!consume(interpreter, str))
    {
        fail(interpreter);
    }
}

void skip(Interpreter *interpreter)
{
    while (isspace(*interpreter->current))
    {
        interpreter->current += 1;
    }
}

bool consume(Interpreter *interpreter, const char *str)
{
    skip(interpreter);

    size_t i = 0;
    while (true)
    {
        char const expected = str[i];
        char const found = interpreter->current[i];
        if (expected == 0)
        {
            /* survived to the end of the expected string */
            interpreter->current += i;
            return true;
        }
        if (expected != found)
        {
            return false;
        }
        // assertion: found != 0
        i += 1;
    }
}

optional_Slice consume_identifier(Interpreter *interpreter)
{
    skip(interpreter);

    if (isalpha(*interpreter->current))
    {
        char const *start = interpreter->current;
        do
        {
            interpreter->current += 1;
        } while (isalnum(*interpreter->current));
        Slice a = {start, (size_t)(interpreter->current - start)};
        return (optional_Slice){true, a};
    }
    else
    {
        Slice a = {"", 0};
        return (optional_Slice){false, a};
    }
}

optional_uint64_t consume_literal(Interpreter *interpreter)
{
    skip(interpreter);

    if (isdigit(*interpreter->current))
    {
        uint64_t v = 0;
        do
        {
            v = 10 * v + ((*interpreter->current) - '0');
            interpreter->current += 1;
        } while (isdigit(*interpreter->current));
        return (optional_uint64_t){true, v};
    }
    else
    {
        return (optional_uint64_t){false, 0};
    }
}

optional_uint64_t get_function_id(Interpreter *interpreter)
{
    skip(interpreter);
    if (isalpha(*interpreter->current))
    {
        char const *start = interpreter->current;
        do
        {
            interpreter->current += 1;
        } while (isalnum(*interpreter->current));
        Slice a = {start, (size_t)(interpreter->current - start)};
        return (optional_uint64_t){true, map_get(&function_table, &a)};
    }
    return (optional_uint64_t){false, 0};
}

uint64_t e0(Interpreter *interpreter, bool effects)
{
    if (consume(interpreter, "fun"))
    {
        
        if (isalnum(*interpreter->current))
        { // consider removing?
            interpreter->current -= 3;
        }
        else
        {
            // try to get function TODO: cehck edge cases
            // that begin with fun
            if (consume(interpreter, "{"))
            {
                if (effects)
                {
                    char *start = interpreter->current;
                    function_list[function_counter] = start;
                }
                while (!consume(interpreter, "}"))
                {
                    statement(interpreter, false);
                }
                if (effects)
                {
                    function_end_list[function_counter] =
                        interpreter->current;
                    function_counter++;
                    return function_counter - 1;
                }
                return 0;
            }
            else
            {
                if (effects)
                {
                    char *start = interpreter->current;
                    function_list[function_counter] = start;
                }
                bool success = statement(interpreter, false);
                if (effects)
                {
                    function_end_list[function_counter] =
                        interpreter->current;
                    // printf("%ld, function_counter,%ld\n", function_counter, function_end_list[function_counter]-function_list[function_counter]);
                    // printf("start, end, %c,%c\n", *function_list[function_counter], *function_end_list[function_counter]);
                    function_counter++;
                    return function_counter - 1;
                }
                if (!success){
                    fail(interpreter);
                }
                return 0;
            }
        }
    }

    optional_Slice id = consume_identifier(interpreter);
    if (id.has_value)
    { // given an id, it could be a normal symbol, a function
      // symbol, or an it, and we need to tell the difference
        uint64_t function_num = map_get(&function_table, &id.value);
        if (function_num)
        {
            return function_num;
        }
        uint64_t v = map_get(&symbol_table, &id.value);
        if (slice_EqualsStr(&id.value, "it") && interpreter->it)
        {
            v = *interpreter->it;
        }
        return v;
    }
    optional_uint64_t v = consume_literal(interpreter);
    if (v.has_value)
    {
        return v.value;
    }
    if (consume(interpreter, "("))
    {
        uint64_t v = expression(interpreter, effects);
        consume(interpreter, ")");
        return v;
    }
    else
    {
        fail(interpreter);
        return 0; // this should never be reached bcs of fail() but im putting
                  // it here to make the compiler happy
    }
}

uint64_t e1(Interpreter *interpreter, bool effects)
{
    uint64_t v = e0(interpreter, effects);
    if (consume(interpreter, "("))
    {
        uint64_t argument = expression(interpreter, effects);
        if (effects)
        {
            Interpreter new_interpreter = {interpreter->program,
                                           function_list[v], &argument, &v};
            return_flag = false;
            // printf("running %ld", argument);
            return_register = 0;
            run(&new_interpreter);
            v = return_register;
        }
        consume(interpreter, ")");
    }
    return v;
}

// ++ -- unary+ unary- ... (Right)
uint64_t e2(Interpreter *interpreter, bool effects)
{
    return e1(interpreter, effects);
}

// * / % (Left)
uint64_t e3(Interpreter *interpreter, bool effects)
{
    uint64_t v = e2(interpreter, effects);
    while (true)
    {
        if (consume(interpreter, "*"))
        {
            v = v * e2(interpreter, effects);
        }
        else if (consume(interpreter, "/"))
        {
            uint64_t right = e2(interpreter, effects);
            v = (right == 0) ? 0 : v / right;
        }
        else if (consume(interpreter, "%"))
        {
            uint64_t right = e2(interpreter, effects);
            v = (right == 0) ? 0 : v % right;
        }
        else
        {
            return v;
        }
    }
}

// (Left) + -
uint64_t e4(Interpreter *interpreter, bool effects)
{
    uint64_t v = e3(interpreter, effects);

    while (true)
    {
        if (consume(interpreter, "+"))
        {
            v = v + e3(interpreter, effects);
        }
        else if (consume(interpreter, "-"))
        {
            v = v - e3(interpreter, effects);
        }
        else
        {
            return v;
        }
    }
}

// << >>
uint64_t e5(Interpreter *interpreter, bool effects)
{
    return e4(interpreter, effects);
}

// < <= > >=
uint64_t e6(Interpreter *interpreter, bool effects)
{
    uint64_t v = e5(interpreter, effects);

    while (true)
    {
        if (consume(interpreter, "<="))
        {
            v = v <= e5(interpreter, effects);
        }
        else if (consume(interpreter, ">="))
        {
            v = v >= e5(interpreter, effects);
        }
        else if (consume(interpreter, "<"))
        {
            v = v < e5(interpreter, effects);
        }
        else if (consume(interpreter, ">"))
        {
            v = v > e5(interpreter, effects);
        }
        else
        {
            return v;
        }
    }
}

// == !=
uint64_t e7(Interpreter *interpreter, bool effects)
{
    uint64_t v = e6(interpreter, effects);

    while (true) // address short circuiting
    {
        if (consume(interpreter, "=="))
        {
            v = v == e6(interpreter, effects);
        }
        else if (consume(interpreter, "!="))
        {
            v = v != e6(interpreter, effects);
        }
        else
        {
            return v;
        }
    }
}

// (left) &
uint64_t e8(Interpreter *interpreter, bool effects)
{
    uint64_t v = e7(interpreter, effects);
    while (true)
    {
        if (consume(interpreter, "&"))
        {
            if (consume(interpreter, "&"))
            {
                interpreter->current -= 2;
                return v;
            }
            else
            {
                v = v & e7(interpreter, effects);
            }
        }
        else
        {
            return v;
        }
    }
}

// ^
uint64_t e9(Interpreter *interpreter, bool effects)
{
    return e8(interpreter, effects);
}

// |
uint64_t e10(Interpreter *interpreter, bool effects)
{
    return e9(interpreter, effects);
}

// &&
uint64_t e11(Interpreter *interpreter, bool effects)
{
    uint64_t v = e10(interpreter, effects);
    while (true)
    {
        if (consume(interpreter, "&&"))
        {
            if (v)
            {
                v = v && e10(interpreter, effects);
            }
            else
            {
                v = 0;
                e10(interpreter, false);
            }
        }
        else
        {
            return v;
        }
    }
}

// ||
uint64_t e12(Interpreter *interpreter, bool effects)
{
    uint64_t v = e11(interpreter, effects);
    while (true)
    {
        if (consume(interpreter, "||"))
        {
            if (v)
            {
                v = 1;
                e11(interpreter, false);
            }
            else
            {
                v = v || e11(interpreter, effects);
            }
        }
        else
        {
            return v;
        }
    }
}

// (right with special treatment for middle expression) ?:
uint64_t e13(Interpreter *interpreter, bool effects)
{
    return e12(interpreter, effects);
}

// = += -= ...
uint64_t e14(Interpreter *interpreter, bool effects)
{
    return e13(interpreter, effects);
}

// ,
uint64_t e15(Interpreter *interpreter, bool effects)
{
    uint64_t v = e14(interpreter, effects);
    while (true)
    {
        if (consume(interpreter, ","))
        {
            v = e14(interpreter, effects);
        }
        else
        {
            return v;
        }
    }
}

uint64_t expression(Interpreter *interpreter, bool effects)
{
    return e15(interpreter, effects);
}

void printLine(Interpreter *interpreter)
{
    char *curr = interpreter->current;
    while (*curr != '\n')
    {
        printf("%c", *curr);
        curr++;
    }
    printf("\n");
}

bool statement(Interpreter *interpreter, bool effects)
{
    // printLine(interpreter);
    if (interpreter->it && interpreter->current >= function_end_list[*interpreter->id] && effects){
        // printf("%s",interpreter->current);
        return false;
    }
    if (consume(interpreter, "print"))
    {
        // print ...
        uint64_t v = expression(interpreter, effects);
        if (effects)
        {
            printf("%ld\n", v);
        }
        return true;
    }
    else if (consume(interpreter, "if"))
    {
        // if ... optional else
        uint64_t v = expression(interpreter, effects);
        if (consume(interpreter, "{"))
        {
            while (!consume(interpreter, "}"))
            {
                if (!statement(interpreter, effects && v))
                {
                    return_flag = 1;
                    return 0;
                } // we do the stuff inside the if
            }
        }
        else
        {
            if (!statement(interpreter, effects && v))
            {
                return_flag = 1;
                return 0;
            }
        }
        if (consume(interpreter, "else"))
        {
            if (consume(interpreter, "{"))
            {
                while (!consume(interpreter, "}"))
                {
                    if (!statement(interpreter, effects && !v))
                    {
                        return_flag = 1;
                        return 0;
                    } // we do the stuff inside the else
                }
            }
            else
            {
                if (!statement(interpreter, effects && !v))
                {
                    return_flag = 1;
                    return 0;
                }
            }
        }
        return true;
    }
    else if (consume(interpreter, "while"))
    {
        char *expression_start = interpreter->current;
        char *end_location;

        uint64_t v = expression(interpreter, effects);
        do
        { // use do while, because if its false the first time we still want
          // to get past the while loop
            if (consume(interpreter, "{"))
            {
                while (!consume(interpreter, "}"))
                {
                    if (!statement(interpreter, effects && v))
                    {
                        return_flag = 1;
                        return 0;
                    } // effects and v both need to be true
                }
            }
            else
            {
                if (!statement(interpreter, effects && v))
                {
                    return_flag = 1;
                    return 0;
                }
            }
            end_location = interpreter->current;
            interpreter->current = expression_start;
            v = expression(interpreter, effects);
        } while (v && effects);
        interpreter->current = end_location;
        return true;
    }
    else if (consume(interpreter, "return"))
    {
        uint64_t v = expression(interpreter, effects);
        if (effects)
        {
            if (interpreter->it)
            {
                return_register = v;
                return false; // we are done with this function
            }
            else
            {
                fail(interpreter);
            }
        }
        return true;
    }
    optional_Slice id = consume_identifier(interpreter);
    if (id.has_value)
    {
        // x = ...
        if (consume(interpreter, "="))
        {

            if (curr_slice)
                free(curr_slice);
            curr_slice = slice_FromSlice(&id.value);
            uint64_t v = expression(interpreter, effects);
            if (effects)
            {
                if (slice_EqualsStr(&id.value, "it") && interpreter->it)
                {
                    *interpreter->it = v;
                }
                else
                {
                    // slice_Print(&id.value);
                    // printf(": %ld\n", v);
                    map_insert(&symbol_table, &id.value, v);
                }
            }
            return true;
        }
        else
        {
            fail(interpreter);
        }
    }
    return false;
}

void statements(Interpreter *interpreter, bool effects)
{
    while (statement(interpreter, effects))
        ;
}

void run(Interpreter *interpreter)
{
    statements(interpreter, true);
    end_or_fail(interpreter);
}

int main(int argc, const char *const *const argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <file name>\n", argv[0]);
        exit(1);
    }

    // open the file
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        exit(1);
    }

    // determine its size (std::filesystem::map_get_size?)
    struct stat file_stats;
    int rc = fstat(fd, &file_stats);
    if (rc != 0)
    {
        perror("fstat");
        exit(1);
    }

    // map the file in my address space
    char *prog =
        (char *)mmap(0, file_stats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (prog == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
    Interpreter interpreter = {prog, prog, NULL, NULL};
    function_table = map_create(1 << 18);
    symbol_table = map_create(1 << 18);
    // no need to create hashmap until we are sure we won't exit,
    // so i don't have to call map_destroy multiple times
    run(&interpreter);
    map_destroy(&symbol_table);
    map_destroy(&function_table);
    if (curr_slice)
        free(curr_slice);

    return 0;
}