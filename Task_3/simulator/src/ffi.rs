#[link(name = "simulator")]
unsafe extern "C" {
    pub fn gpu_execute(
        shader_src: *const u8,
        shader_len: u64,
        tree_grid_size: u64
    ) -> u64;
}