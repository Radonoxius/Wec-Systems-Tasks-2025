//0 = on fire
//1 = alive
//2 = burnt

kernel void solver(
    global uint *dimension,
    global uchar *tree_grid,
    global uchar *randoms,
    global uchar *next_tree_grid
) {
    private size_t x = get_global_id(0);
    private size_t y = get_global_id(1);
}