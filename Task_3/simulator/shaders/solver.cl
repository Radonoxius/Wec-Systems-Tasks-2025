//0 = on fire
//1 = alive
//2 = burnt

void process_corners(
    size_t x,
    size_t y,
    global uint *dimension,
    global uchar *tree_grid,
    global uchar *randoms,
    global uchar *next_tree_grid
) {
    private size_t index = dimension * y + x;
    private uchar will_burn = 0;

    if(x == 0 && y == 0) {
        will_burn = randoms[index] ^
            (tree_grid[1] & tree_grid[dimension] & tree_grid[dimension + 1]);
    }
    else if(x == (*dimension - 1) && y == 0) {
        will_burn = randoms[index] ^
            (tree_grid[index - 1] & tree_grid[index + dimension] & tree_grid[(index + dimension) - 1]);
    }
    else if(x == 0 && y == (*dimension - 1)) {
        will_burn = randoms[index] ^
            (tree_grid[index + 1] & tree_grid[index - dimension] & tree_grid[(index - dimension) + 1]);
    }
    else if(x == (*dimension - 1) && y == (*dimension - 1)) {
        will_burn = randoms[index] ^
            (tree_grid[index - 1] & tree_grid[index - dimension] & tree_grid[(index - dimension) - 1]);
    }

    if(will_burn == 1)
        next_tree_grid[index] = 0;
    else
        next_tree_grid[index] = tree_grid[index];
}

void process_edges(
    size_t x,
    size_t y,
    global uint *dimension,
    global uchar *tree_grid,
    global uchar *randoms,
    global uchar *next_tree_grid
) {
    private size_t index = dimension * y + x;
    private uchar will_burn = 0;

    if(x == 0 && (y != 0 && y != (*dimension - 1))) {
        will_burn = randoms[index] ^
            (tree_grid[index + 1] & tree_grid[index - dimension] & tree_grid[index + dimension] & tree_grid[(index - dimension) + 1] & tree_grid[index + dimension + 1]);
    }
    else if(x == (*dimension - 1) && (y != 0 && y != (*dimension - 1))) {
        
    }

    if(will_burn == 1)
        next_tree_grid[index] = 0;
    else
        next_tree_grid[index] = tree_grid[index];
}

kernel void solver(
    global uint *dimension,
    global uchar *tree_grid,
    global uchar *randoms,
    global uchar *next_tree_grid
) {
    private size_t x = get_global_id(0);
    private size_t y = get_global_id(1);

    process_corners(x, y, dimension, tree_grid, randoms, next_tree_grid);

    process_edges(x, y, dimension, tree_grid, randoms, next_tree_grid);
}