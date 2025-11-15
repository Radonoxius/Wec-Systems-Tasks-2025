use std::{ops::RangeInclusive, sync::{Arc, atomic::{AtomicBool, AtomicU64, Ordering}}, thread::{self, JoinHandle}};

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

pub fn solve_hash_parallel(
    f: fn(
        Arc<AtomicBool>,
        Arc<AtomicU64>
    ) -> Vec<JoinHandle<()>>
) {
    hash_2_range();
    let success = Arc::new(AtomicBool::new(false));
    let count = Arc::new(AtomicU64::new(0));

    let join_handles = f(success, count);

    for handle in join_handles {
        handle.join().unwrap();
    }
}

pub fn generate_loop_range1(index: u8) -> RangeInclusive<char> {
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
    success: &Arc<AtomicBool>,
    completion_count: &Arc<AtomicU64>
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

                                        completion_count1.fetch_add(1, Ordering::Relaxed);

                                        if success1.load(Ordering::Relaxed) {
                                            break 'outer;
                                        }

                                        if equals_hash_1(&current_word) {
                                            println!("\nThe secret word is: {}", &current_word);
                                            success1.store(true, Ordering::Release);
                                            break 'outer;
                                        }
                                    }
                                }
                            }

                            print!(
                                "\rTheoretical progress: {}/308915776",
                                completion_count1.load(Ordering::Acquire)
                            );
                        }
                    }
                }
            }
        )
    );
}

pub fn hash_2_range() -> [char; 52] {
    let mut range = ['a'; 52];

    let mut idx = 0;
    for ci in 'a'..='z' {
        range[idx] = ci;
        idx += 1;
    }
    for cj in 'A'..='Z' {
        range[idx] = cj;
        idx +=1;
    }

    range
}

pub fn generate_loop_range2(index: u8) -> RangeInclusive<char> {
    match index {
        0 => 'a'..='z',
        1 => 'A'..='Z',
        _ => panic!("Index can only be a 0 or 1!")
    }
}

pub fn hash_2_parallel_job(
    join_handles: &mut Vec<JoinHandle<()>>,
    loop4_range: RangeInclusive<char>,
    loop5_range: RangeInclusive<char>,
    loop6_range: RangeInclusive<char>,
    success: &Arc<AtomicBool>,
    completion_count: &Arc<AtomicU64>
) {
    let success1 = success.clone();
    let completion_count1 = completion_count.clone();

    join_handles.push(
        thread::spawn(
            move || {
                'outer: for c1 in hash_2_range() {
                    for c2 in hash_2_range() {
                        for c3 in hash_2_range() {
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

                                        completion_count1.fetch_add(1, Ordering::Relaxed);

                                        if success1.load(Ordering::Relaxed) {
                                            break 'outer;
                                        }

                                        if equals_hash_2(&current_word) {
                                            println!("\nThe secret word is: {}", &current_word);
                                            success1.store(true, Ordering::Release);
                                            break 'outer;
                                        }
                                    }
                                }
                            }

                            print!(
                                "\rTheoretical progress: {}/19770609664",
                                completion_count1.load(Ordering::Acquire)
                            );
                        }
                    }
                }
            }
        )
    );
}

//'system' is the answer :-)
//HASH1 is obtained from 6 letter word that has lowercase alphabets only
pub fn equals_hash_1(string: &String) -> bool {
    hex!(
        "bbc5e661e106c6dcd8dc6dd186454c2fcba3c710fb4d8e71a60c93eaf077f073"
    )[..] == Sha256::digest(&string.as_bytes())[..]
}

//'kerNEl' is the answer :-)
//HASH2 is obtained from 6 letter word that has both lower and upper case alphabets
pub fn equals_hash_2(string: &String) -> bool {
    hex!(
        "502eadebf906967ad022cb7b4553b867f245770595e94df8d475b5a48eaaf434"
    )[..] == Sha256::digest(&string.as_bytes())[..]
}