use std::time::Instant;

use sha2::{Digest, Sha256};
use shracker::{solve_hash_1, HASH1};

fn main() {
    let start_time = Instant::now();

    solve_hash_1(|
        word,
        success_flag_ref,
        count_ref
        | -> String {
        let mut current_word = String::with_capacity(6);

        'inner: for c4 in 'a'..='z' {
            for c5 in 'a'..='z' {
                for c6 in 'a'..='z' {
                    current_word = word.clone();
                    current_word.push(c4);
                    current_word.push(c5);
                    current_word.push(c6);

                    *count_ref += 1;

                    if HASH1 == Sha256::digest(&current_word)[..] {
                        println!("The word is: {}", &current_word);
                        *success_flag_ref = true;
                        break 'inner;
                    }
                }
            }
        }

        current_word
    });

    let duration = start_time.elapsed();
    println!("The computation took: {} s", duration.as_secs());
}