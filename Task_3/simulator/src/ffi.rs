#[link(name = "simulator")]
unsafe extern "C" {
    pub(crate) fn gpu_execute(
        shader_src: *const u8,
        shader_len: u64,
        global_work_size: *const u8,
        local_work_size: *const u8
    );
}