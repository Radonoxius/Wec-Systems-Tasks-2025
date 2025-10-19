use std::{ops::RangeInclusive, sync::{Arc, Mutex, RwLock}, thread::{self, JoinHandle}};

use hex_literal::hex;
use sha2::{Digest, Sha256};

pub fn solve_hash_1_seqential(f: fn(String, &mut bool, &mut u64)) {
    let mut success = false;
    let mut count = 0u64;

    'outer: for c1 in 'a'..='z' {
        for c2 in 'a'..='z' {
            for c3 in 'a'..='z' {
                let mut word = String::with_capacity(6);
                    word.push(c1);
                    word.push(c2);
                    word.push(c3);

                f(word, &mut success, &mut count);
                print!("\rTheoretical progress: {}/308915776", &count);

                if success {
                    break 'outer;
                }
            }
        }
    }
}

pub fn solve_hash_1_parallel(
    f: fn(
        Arc<RwLock<bool>>,
        Arc<Mutex<u64>>
    ) -> Vec<JoinHandle<()>>
) {
    let success = Arc::new(RwLock::new(false));
    let count = Arc::new(Mutex::new(0u64));

    let join_handles = f(success, count);

    for handle in join_handles {
        handle.join().unwrap();
    }
}

pub fn generate_loop_range(index: u8) -> RangeInclusive<char> {
    match index {
        0 => 'a'..='m',
        1 => 'n'..='z',
        _ => panic!("Index can only be a 0 or 1!")
    }
}

pub fn hash_1_parallel_job(
    join_handles: &mut Vec<JoinHandle<()>>,
    loop4_range: RangeInclusive<char>,
    loop5_range: RangeInclusive<char>,
    loop6_range: RangeInclusive<char>,
    success: &Arc<RwLock<bool>>,
    completion_count: &Arc<Mutex<u64>>
) {
    let success1 = success.clone();
    let completion_count1 = completion_count.clone();

    join_handles.push(
        thread::spawn(
            move || {
                'outer: for c1 in 'a'..='z' {
                    for c2 in 'a'..='z' {
                        for c3 in 'a'..='z' {
                            for c4 in loop4_range.clone() {
                                for c5 in loop5_range.clone() {
                                    for c6 in loop6_range.clone() {
                                        let mut current_word = String::with_capacity(6);
                                        current_word.push(c1);
                                        current_word.push(c2);
                                        current_word.push(c3);
                                        current_word.push(c4);
                                        current_word.push(c5);
                                        current_word.push(c6);

                                        *completion_count1.lock().unwrap() += 1;

                                        if *success1.read().unwrap() {
                                            break 'outer;
                                        }

                                        if equals_hash_1(&current_word) {
                                            println!("\nThe secret word is: {}", &current_word);
                                            *success1.write().unwrap() = true;
                                            break 'outer;
                                        }
                                    }
                                }
                            }

                            if !*success1.read().unwrap() {
                                print!(
                                    "\rTheoretical progress: {}/308915776",
                                    completion_count1.lock().unwrap()
                                );
                            }
                        }
                    }
                }
            }
        )
    );
}


pub fn solve_hash_2_parallel(_f: fn(String, &mut bool, &mut u64) -> String) {
    todo!()
}

//'system' is the answer :-)
//HASH1 is obtained from 6 letter word that has lowercase alphabets only
pub fn equals_hash_1(string: &String) -> bool {
    hex!(
        "bbc5e661e106c6dcd8dc6dd186454c2fcba3c710fb4d8e71a60c93eaf077f073"
    )[..] == Sha256::digest(&string.as_bytes())[..]
}

//HASH2 is obtained from 7 letter word that has both lower and upper case alphabets
pub fn equals_hash_2(string: &String) -> bool {
    hex!(
        "8d7ed7808ef4c15a1bfb45406d7fafdf6afa48928afb88a64baa2c849296ac6d"
    )[..] == Sha256::digest(&string.as_bytes())[..]
}