use std::time::Instant;

use shracker::{equals_hash_1, solve_hash_1_seqential};

fn main() {
    let start_time = Instant::now();

    solve_hash_1_seqential(|
            word,
            success_flag_ref,
            count_ref
        | {
            #[allow(unused_assignments)]
            let mut current_word = String::with_capacity(6);

            'inner: for c4 in 'a'..='z' {
                for c5 in 'a'..='z' {
                    for c6 in 'a'..='z' {
                        current_word = word.clone();
                        current_word.push(c4);
                        current_word.push(c5);
                        current_word.push(c6);

                        *count_ref += 1;

                        if equals_hash_1(current_word.as_bytes()) {
                            println!("\nThe secret word is: {}", &current_word);
                            *success_flag_ref = true;
                            break 'inner;
                        }
                    }
                }
            }
        }
    );

    let duration = start_time.elapsed();
    println!("\nThe computation took: {} s", duration.as_secs());
}