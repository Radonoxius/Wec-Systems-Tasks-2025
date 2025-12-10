use simulator::{ffi::simulate, read_shader};

const TREE_GRID_SIZE: u32 = 50;

//starting_factor = 255 means all trees start off in alive state
//starting_factor = 0 means all trees start off in burning state
const STARTING_FACTOR: u8 = 100;

fn main() {
    let shader_src = read_shader();

    let epoch_count = unsafe {
        simulate(
            shader_src.as_ptr(),
            shader_src.len() as u64,
            TREE_GRID_SIZE,
            STARTING_FACTOR
        )
    };

    println!("The forest burnt completely in: {} epochs.", epoch_count);
}