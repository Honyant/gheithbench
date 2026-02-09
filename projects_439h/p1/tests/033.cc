#include "debug.h"
#include "critical.h"
#include "atomic.h"

/*
This testcase is a simple example of using critical sections to generate Pascal's Triangle. It tests nested criticals,
and ensures that ordering is right, due the pascal triangle's of each core building off the previously generated triangle.
*/

const int MAX_ROWS = 16;
const int MAX_COLS = 16;
const int rowsPerCore = 4;

static uint32_t counter = 0;
static int triangle[MAX_ROWS][MAX_COLS];
static int beginRow = 0;
int currentRow = 0;

// Function to calculate a specific row of Pascal's Triangle
void genRow()
{
    int row = beginRow + currentRow;
    int endRow = beginRow + rowsPerCore;
    if (row < endRow)
    {
        triangle[row][0] = 1;
        for (int col = 1; col <= row; ++col)
        {
            triangle[row][col] = triangle[row - 1][col - 1] + triangle[row - 1][col];
        }

        if (row + 1 < endRow)
        {
            currentRow += 1;
            critical(genRow);
        }
    }
}
void printTriangle()
{
    Debug::printf("*** Pascal's Triangle: Core %d\n", counter + 1);
    for (int row = 0; row < beginRow + rowsPerCore; row++)
    {
        Debug::printf("*** Row %d: ", row + 1);
        for (int col = 0; col <= row; ++col)
        {
            Debug::printf("*** %d ", triangle[row][col]);
        }
        Debug::printf("\n");
    }
}

void createPascal()
{
    triangle[0][0] = 1;
    currentRow = 0;
    critical(genRow);
    critical(printTriangle);
    beginRow += rowsPerCore;
    counter += 1;
}

/* Called by all cores */
void kernelMain(void)
{
    critical(createPascal);
}
