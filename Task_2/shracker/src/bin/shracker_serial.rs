use std::time::Instant;

use shracker::{generate_sha256_hash, HASH1};

fn main() {
    let start_time = Instant::now();

    'outer: for c1 in 'a'..='z' {
        for c2 in 'a'..='z' {
            for c3 in 'a'..='z' {
                for c4 in 'a'..='z' {
                    for c5 in 'a'..='z' {
                        for c6 in 'a'..='z' {
                            let mut word = String::with_capacity(6);
                            word.push(c1);
                            word.push(c2);
                            word.push(c3);
                            word.push(c4);
                            word.push(c5);
                            word.push(c6);

                            if HASH1 == generate_sha256_hash(&word) {
                                println!("The word is: {}", word);
                                break 'outer;
                            }
                        }
                    }
                }
            }
        }
    }

    let duration = start_time.elapsed();
    println!("The computation took: {} s", duration.as_secs());
}