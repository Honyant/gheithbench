#include "debug.h"
#include "critical.h"
#include "heap.h"

/* 
 * This test case uses a highly (in fact, excessively) concurrent algorithm to simulate Conway's Game of Life.
 * critical() is called at least (2 * rows * cols * iters) times in total.
 * A large portion of those calls are truly critical, causing incorrect grid state if two cores, say, try to move at once.
 * This game rigorously tests parallelization because inaccurate intermediate states are likely to cause persistent differences.
 * Due to an implementation quirk, any positive number of cores might compute the new top left cell on the first iteration (which doesn't affect correctness).
*/ 

/* 
 * Rules for Conway's Game of Life:
 * There exists a 2-dimensional grid of cells that changes on discrete time steps; each cell is either live or dead at any given time.
 * In each time step, all cells are updated simultaneously based on the states of nearby cells.
 * Cells become live if either they and 2-3 of their neighbors were live, or they were dead and exactly 3 of their neighbors were live.
 * My grid is toroidal: the top wraps around to the bottom and the left wraps around to the right (and vice versa).
 * The initial configuration I used is called a Gosper glider gun: it creates 5-cell entities that glide across the grid until they wrap around and crash into the gun.
 * The infinite-grid version of the game is Turing complete: it can simulate a Turing machine.
*/

// coordinate pair
struct Pair {
    uint16_t y;
    uint16_t x;
};

static const uint16_t rows = 19; // grid height
static const uint16_t cols = 45; // grid width
static const uint16_t iters = 56; // number of time steps executed
static const uint16_t vc = rows/2;
static const uint16_t hc = cols/2;
// the initial live cells
static const Pair gun[] = {{vc, hc - 17}, {vc - 1, hc - 17}, {vc, hc - 16}, {vc - 1, hc - 16},           
                            {vc, hc - 7}, {vc - 1, hc - 7}, {vc - 2, hc - 7}, {vc + 1, hc - 6}, {vc - 3, hc - 6}, 
                            {vc + 2, hc - 5}, {vc - 4, hc - 5}, {vc + 2, hc - 4}, {vc - 4, hc - 4}, 
                            {vc - 1, hc - 3}, {vc + 1, hc - 2}, {vc - 3, hc - 2}, 
                            {vc, hc - 1}, {vc - 1, hc - 1}, {vc - 2, hc - 1}, {vc - 1, hc}, 
                            {vc + 2, hc + 3}, {vc + 1, hc + 3}, {vc, hc + 3}, 
                            {vc + 2, hc + 4}, {vc + 1, hc + 4}, {vc, hc + 4}, 
                            {vc + 3, hc + 5}, {vc - 1, hc + 5},
                            {vc + 4, hc + 7}, {vc + 3, hc + 7}, {vc - 1, hc + 7}, {vc - 2, hc + 7},
                            {vc + 2, hc + 17}, {vc + 1, hc + 17}, {vc + 2, hc + 18}, {vc + 1, hc + 18}}; 

static uint8_t old[rows][cols] = {0};
static uint8_t current[rows][cols];
static bool copying = false;
static bool initialized = false;
static bool done = false;
static bool displayed = false;
static uint16_t row = 0;
static uint16_t col = 0;
static PerCPU<uint16_t> myrow;
static PerCPU<uint16_t> mycol;
static PerCPU<bool> working; 
static uint16_t iter = 0;

// simple pseudo-barrier implementation: returns whether some other core is still working on its cell
bool somebodyWorking() {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (working.forCPU(i)) {
            return true; // CPU i is working
        }
    }
    return false; // if reached, no CPUs are working
}

// sets the grid to the correct initial state (glider gun)
void initialize() {
    if (initialized) {
        return; // only need to do this once
    }
    for (uint32_t i = 0; i < sizeof(gun)/sizeof(Pair); i++) {
        old[gun[i].y][gun[i].x] = true;
    }
    initialized = true; // tell other cores that initialization is done
}

// figures out the next task to do
void move() {
    if (done) {
        return;
    }
    // move one cell right
    col++;
    if (col == cols) {
        col -= cols;
        // move one row down
        row++;
        if (row == rows) {
            row -= rows;
            // switch between copying back and computing next time step
            while (somebodyWorking()); // wait for other cores to finish their last cells
            copying = !copying; 
            if (!copying) {
                iter++;
                if (iter >= iters) {
                    // start printing phase
                    while (somebodyWorking()); // wait for other cores to finish their last cells
                    done = true;
                }
            }
        }
    }
    // assign this CPU its next cell
    mycol.mine() = col;
    myrow.mine() = row;
    working.mine() = true;
}

// tests that calls to critical() inside critical sections work properly
void moveWrap() {
    critical(move);
}

// either updates or copies back a cell
void work() {
    while (!done) {
        // myrow and mycol don't change when other cores move
        uint16_t row = myrow.mine();
        uint16_t col = mycol.mine();
        if (copying) {
            // write cell back to old grid
            old[row][col] = current[row][col]; 
        } else {
            // update cell in new grid
            uint16_t u = (row == 0) ? (rows - 1) : row - 1;
            uint16_t d = (row == rows - 1) ? 0 : row + 1;
            uint16_t l = (col == 0) ? (col - 1) : col - 1;
            uint16_t r = (col == cols - 1) ? 0 : col + 1;
            uint8_t sum = old[u][l] + old[u][col] + old[u][r] + old[row][l] + old[row][r] + old[d][l] + old[d][col] + old[d][r];
            current[row][col] = (sum == 3) || ((sum == 2) && old[row][col]);
        }
        working.mine() = false; // tell other cores that this CPU is done with its cell
        critical(moveWrap); // called about (2 * rows * cols * iters) total times across the four cores
    }
}

// prints final grid state
void display() {
    if (displayed) {
        return; // must do this exactly once
    }
    row = 0;
    col = 0;
    // loop through grid and print one row at a time
    while (row < rows) {
        uint32_t i = 0;
        char* rowstr = (char*) malloc((cols + 1) * sizeof(char));
        while (col < cols) {
            rowstr[i] = old[row][col] ? 'O' : '.'; // live cells are 'O'; dead cells are '.'
            col++;
            i++;
        }
        Debug::printf("*** %s\n", (char*) rowstr);
        row++;
        col -= cols;
    }
    displayed = true; // tell other cores that displaying is done
}

void kernelMain() {
    critical(initialize); // hard to parallelize: complex initial pattern
    work(); // can be largely done in parallel
    critical(display); // hard to parallelize: prints would occur out of order
}
