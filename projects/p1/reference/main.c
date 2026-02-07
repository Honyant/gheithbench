#include "hashmap.h"
#include "slice.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>



typedef struct //replicate std::optional
{
    bool has_value;
    uint64_t value;
} optional_uint64_t;

typedef struct //replicate std::optional
{
    bool has_value;
    Slice value;
} optional_Slice;

const char* program;
char const* current;
Hashmap symbol_table;

bool consume(const char* str);
uint64_t expression(bool effects);

void fail()
{
    printf("failed at offset %ld\n", (size_t)(current - program));
    printf("%s\n", current);
    map_destroy(&symbol_table);
    exit(1);
}

void end_or_fail()
{
    while (isspace(*current))
    {
        current += 1;
    }
    if (*current != 0)
        fail();
}

void consume_or_fail(const char* str)
{
    if (!consume(str))
    {
        fail();
    }
}

void skip()
{
    while (isspace(*current))
    {
        current += 1;
    }
}

bool consume(const char* str)
{
    skip();

    size_t i = 0;
    while (true)
    {
        char const expected = str[i];
        char const found = current[i];
        if (expected == 0)
        {
            /* survived to the end of the expected string */
            current += i;
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

optional_Slice consume_identifier()
{
    skip();

    if (isalpha(*current))
    {
        char const* start = current;
        do
        {
            current += 1;
        } while (isalnum(*current));
        Slice a = { start, (size_t)(current - start) };
        return (optional_Slice) { true, a };
    }
    else
    {
        Slice a = { "", 0 };
        return (optional_Slice) { false, a };
    }
}

optional_uint64_t consume_literal()
{
    skip();

    if (isdigit(*current))
    {
        uint64_t v = 0;
        do
        {
            v = 10 * v + ((*current) - '0');
            current += 1;
        } while (isdigit(*current));
        return (optional_uint64_t) { true, v };
    }
    else
    {
        return (optional_uint64_t) { false, 0 };
    }
}

uint64_t e1(bool effects)
{
    optional_Slice id = consume_identifier();
    if (id.has_value)
    {
        uint64_t v = map_get(&symbol_table, &id.value);
        return v;
    }
    optional_uint64_t v = consume_literal();
    if (v.has_value)
    {
        return v.value;
    }
    if (consume("("))
    {
        uint64_t v = expression(effects);
        consume(")");
        return v;
    }
    else
    {
        fail();
        return 0; // this should never be reached bcs of fail() but im putting it here to make the compiler happy
    }
}

// ++ -- unary+ unary- ... (Right)
uint64_t e2(bool effects)
{
    return e1(effects);
}

// * / % (Left)
uint64_t e3(bool effects)
{
    uint64_t v = e2(effects);

    while (true)
    {
        if (consume("*"))
        {
            v = v * e2(effects);
        }
        else if (consume("/"))
        {
            uint64_t right = e2(effects);
            v = (right == 0) ? 0 : v / right;
        }
        else if (consume("%"))
        {
            uint64_t right = e2(effects);
            v = (right == 0) ? 0 : v % right;
        }
        else
        {
            return v;
        }
    }
}

// (Left) + -
uint64_t e4(bool effects)
{
    uint64_t v = e3(effects);

    while (true)
    {
        if (consume("+"))
        {
            v = v + e3(effects);
        }
        else if (consume("-"))
        {
            v = v - e3(effects);
        }
        else
        {
            return v;
        }
    }
}

// << >>
uint64_t e5(bool effects)
{
    return e4(effects);
}

// < <= > >=
uint64_t e6(bool effects)
{
    return e5(effects);
}

// == !=
uint64_t e7(bool effects)
{
    return e6(effects);
}

// (left) &
uint64_t e8(bool effects)
{
    return e7(effects);
}

// ^
uint64_t e9(bool effects)
{
    return e8(effects);
}

// |
uint64_t e10(bool effects)
{
    return e9(effects);
}

// &&
uint64_t e11(bool effects)
{
    return e10(effects);
}

// ||
uint64_t e12(bool effects)
{
    return e11(effects);
}

// (right with special treatment for middle expression) ?:
uint64_t e13(bool effects)
{
    return e12(effects);
}

// = += -= ...
uint64_t e14(bool effects)
{
    return e13(effects);
}

// ,
uint64_t e15(bool effects)
{
    return e14(effects);
}

uint64_t expression(bool effects)
{
    return e15(effects);
}

bool statement(bool effects)
{
    if (consume("print"))
    {
        // print ...
        uint64_t v = expression(effects);
        if (effects)
        {
            printf("%ld\n", v);
        }
        return true;
    }
    optional_Slice id = consume_identifier();
    if (id.has_value)
    {
        // x = ...
        if (consume("="))
        {
            uint64_t v = expression(effects);
            if (effects)
            {
                map_insert(&symbol_table, &id.value, v);
            }
            return true;
        }
        else
        {
            fail();
        }
    }
    return false;
}

void statements(bool effects)
{
    while (statement(effects))
        ;
}

void run()
{
    statements(true);
    end_or_fail();
}

int main(int argc, const char* const* const argv)
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
    char const* prog = (char const*)mmap(
        0,
        file_stats.st_size,
        PROT_READ,
        MAP_PRIVATE,
        fd,
        0);
    if (prog == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    program = prog;
    current = prog;
    symbol_table = map_create(1<<13); //no need to create hashmap until we are sure we won't exit, so i don't have to call map_destroy multiple times
    run();
    map_destroy(&symbol_table);
    return 0;
}