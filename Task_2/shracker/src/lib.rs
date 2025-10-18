//HASH1 is obtained from 6 letter word that has lowercase alphabets only
pub const HASH1: [u8; 64] = *b"bbc5e661e106c6dcd8dc6dd186454c2fcba3c710fb4d8e71a60c93eaf077f073";

//HASH2 is obtained from 7 letter word that has both lower and upper case alphabets
pub const HASH2: [u8; 64] = *b"8d7ed7808ef4c15a1bfb45406d7fafdf6afa48928afb88a64baa2c849296ac6d";

pub fn solve_hash_1(f: fn(String, &mut bool, &mut u64) -> String) -> String {
    let mut success = false;
    let mut result = String::new();
    let mut count = 0u64;

    'outer: for c1 in 'a'..='z' {
        for c2 in 'a'..='z' {
            for c3 in 'a'..='z' {
                let mut word = String::with_capacity(6);
                    word.push(c1);
                    word.push(c2);
                    word.push(c3);

                result = f(word, &mut success, &mut count);
                print!("Theoretical progress: {}/308,915,776\r", &count);

                if success {
                    break 'outer;
                }
            }
        }
    }

    result
}