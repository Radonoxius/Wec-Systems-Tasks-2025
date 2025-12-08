use simulator::{ffi::simulate, read_shader};

const TREE_GRID_SIZE: u32 = 5;

fn main() {
    let shader_src = read_shader();

    let epoch_count = unsafe {
        simulate(
            shader_src.as_ptr(),
            shader_src.len() as u64,
            TREE_GRID_SIZE
        )
    };

    println!("The forest burnt completely in: {} epochs.", epoch_count);
}