/* Tree grid Conventions:
 * 0 = on fire
 * 1 = alive
 * 3 = burnt
 */

kernel void simulate(
    global const uint *dimension, // = N
    global const uchar *tree_grid, //N+2 x N+2
    global const uchar *randoms, //N x N, p(1) = 0.25, p(0) = 0.75
    global uchar *next_tree_grid, //N+2 x N+2
    volatile global atomic_uint *done //0 means no trees left on fire, simulation ends
) {
    private uint x = get_global_id(0);
    private uint y = get_global_id(1);

    private uint idx = *dimension * y + x;
    private uint extended_idx = idx + *dimension + (y << 1) + 3;

    if(tree_grid[extended_idx] == 0 || tree_grid[extended_idx] == 3) {
        next_tree_grid[extended_idx] = 3;
    }
    else {
        private uchar3 lo = vload3(extended_idx - (*dimension + 3), tree_grid);
        private uchar3 mid = vload3(extended_idx - 1, tree_grid);
        private uchar3 hi = vload3(extended_idx + (*dimension + 1), tree_grid);

        //Tree can burn if this is 1
        private uchar can_burn = !(
            lo.s0  & lo.s1  & lo.s2  &
            mid.s0          & mid.s2 &
            hi.s0  & hi.s1  & hi.s2
        );

        //Tree catches fire if randoms is 1 and if can_burn is 1
        next_tree_grid[extended_idx] = !(randoms[idx] & can_burn);
        
        atomic_fetch_add_explicit(
            done,
            randoms[idx] & can_burn,
            memory_order_relaxed,
            memory_scope_work_item
        );
    }
}