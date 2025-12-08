/* Conventions:
 * 0 = on fire
 * 1 = alive
 * 2 = burnt
 */

/* Randoms is an NxN grid of
 * 1s and 0s
 */

kernel void simulate(
    global uint *dimension, //N
    global const uchar *tree_grid, //N+2 x N+2
    global uchar *randoms, //N x N
    global uchar *next_tree_grid, //N+2 x N+2
    volatile global uint *done //0 means no trees left on fire
) {
    private size_t x = get_global_id(0);
    private size_t y = get_global_id(1);
    private size_t actual_idx = (*dimension + 2) * (y + 1) + x + 1;

    if(tree_grid[actual_idx] == 0 || tree_grid[actual_idx] == 2) {
        next_tree_grid[actual_idx] = 2;
    }
    else {
        private uchar3 lo = vload3(actual_idx - (*dimension + 3), tree_grid);
        private uchar3 mid = vload3(actual_idx - 1, tree_grid);
        private uchar3 hi = vload3(actual_idx + (*dimension + 1), tree_grid);
        private size_t relative_idx = *dimension * y + x;

        //Tree can burn if this is 0
        private uchar cannot_burn = (
            lo.s0 & lo.s1 & lo.s2 &
            mid.s0 & mid.s2 &
            hi.s0 & hi.s1 & hi.s2
        );

        next_tree_grid[actual_idx] = !randoms[relative_idx] | cannot_burn;
    }

    if(next_tree_grid[actual_idx] == 0)
        atomic_add(done, 1);
}