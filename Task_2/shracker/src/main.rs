use std::{env::args, thread::JoinHandle, time::Instant};

use shracker::{hash_1_parallel_job, hash_2_parallel_job, solve_hash_parallel};

fn main() {
    let hash_selector = args().nth(1)
        .expect("\nPlease send an argument to specify the hash to be cracked.\nPass 1 to crack hash 1 or 2 to crack hash 2.\n")
        .parse::<u8>()
        .unwrap();

    match hash_selector {
        1 => {
            let start_time = Instant::now();

            solve_hash_parallel(|
                    success,
                    completion_count
                | -> Vec<JoinHandle<()>> {
                    let mut join_handles = Vec::<JoinHandle<()>>::new();

                    for i in 0..=1 {
                        for j in 0..=1 {
                            for k in 0..=1 {
                                hash_1_parallel_job(
                                    &mut join_handles,
                                    i,
                                    j,
                                    k,
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
            println!("\nThe computation took: {} s", duration.as_secs());
        },

        2 => {
            let start_time = Instant::now();

            solve_hash_parallel(|
                    success,
                    completion_count
                | -> Vec<JoinHandle<()>> {
                    let mut join_handles = Vec::<JoinHandle<()>>::new();

                    for i in 0..=1 {
                        for j in 0..=1 {
                            for k in 0..=1 {
                                hash_2_parallel_job(
                                    &mut join_handles,
                                    i,
                                    j,
                                    k,
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
            println!("\nThe computation took: {} s", duration.as_secs());
        },

        _ => panic!("\nPass 1 to crack hash 1 or 2 to crack hash 2 as a program argument\n")
    }
}