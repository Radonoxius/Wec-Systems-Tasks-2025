use hex_literal::hex;
use sha2::{Digest, Sha256};

//'system' is the answer :-)
//HASH1 is obtained from 6 letter word that has lowercase alphabets only
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


pub fn solve_hash_2_parallel(_f: fn(String, &mut bool, &mut u64) -> String) {
    todo!()
}

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