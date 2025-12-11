use simulator::{ffi::simulate, read_shader};

const TREE_GRID_SIZE: u32 = 18000;

//starting_factor = 255 means all trees start off in alive state
//starting_factor = 0 means all trees start off in burning state
const STARTING_FACTOR: u8 = 220;

fn main() {
    let shader_src = read_shader();
    println!("CONVENTION: 0 -> Burning tree, 1 -> Alive n well, 3 -> Burnt.");

    let epoch_count = unsafe {
        simulate(
            shader_src.as_ptr(),
            shader_src.len() as u64,
            TREE_GRID_SIZE,
            STARTING_FACTOR
        )
    };

    println!("The simulation ended in: {} epochs.", epoch_count);
    println!("Burnt partially or fully (could be nothing too...)");
}