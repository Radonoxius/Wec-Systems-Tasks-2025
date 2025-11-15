use simulator::{ffi::gpu_execute, read_shader};

const TREE_GRID_SIZE: u64 = 5;

fn main() {
    let shader_src = read_shader();

    let epoch_count = unsafe {
        gpu_execute(
            shader_src.as_ptr(),
            shader_src.len() as u64,
            TREE_GRID_SIZE
        )
    };

    println!("The forest burnt completely in: {} epochs.", epoch_count);
}