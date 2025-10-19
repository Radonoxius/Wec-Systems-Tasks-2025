use std::{thread::JoinHandle, time::Instant};

use shracker::{generate_loop_range, hash_1_parallel_job, solve_hash_1_parallel};

fn main() {
    let start_time = Instant::now();

    solve_hash_1_parallel(|
            success,
            completion_count
        | -> Vec<JoinHandle<()>> {
            let mut join_handles = Vec::<JoinHandle<()>>::new();

            for i in 0..=1 {
                for j in 0..=1 {
                    for k in 0..=1 {
                        hash_1_parallel_job(
                            &mut join_handles,
                            generate_loop_range(i),
                            generate_loop_range(j),
                            generate_loop_range(k),
                            &success,
                            &completion_count
                        );
                    }
                }
            }

            join_handles
        }
    );

    let duration = start_time.elapsed();
    println!("The computation took: {} s", duration.as_secs());
}