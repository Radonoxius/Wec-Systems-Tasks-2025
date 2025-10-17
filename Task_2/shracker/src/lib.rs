use std::{io::Write, process::{Command, Stdio}};

//HASH1 is obtained from 6 letter word that has lowercase alphabets only
pub const HASH1: &str = "bbc5e661e106c6dcd8dc6dd186454c2fcba3c710fb4d8e71a60c93eaf077f073";

//HASH2 is obtained from 7 letter word that has both lower and upper case alphabets
pub const HASH2: &str = "8d7ed7808ef4c15a1bfb45406d7fafdf6afa48928afb88a64baa2c849296ac6d";

pub fn generate_sha256_hash(string: &str) -> String {
    let mut sha256sum_handle = Command::new("sha256sum")
        .stdin(Stdio::piped())
        .stdout(Stdio::piped())
        .spawn()
        .expect("sha256sum command line utility was not found! Please install it!");

    let sha256sum_stdin = sha256sum_handle.stdin
        .as_mut()
        .expect("Unexpected error while opening sha256sum stdin");

    sha256sum_stdin.write_all(string.as_bytes())
        .expect("Unexpected error while writing to sha256sum stdin");

    let sha256sum_handle = sha256sum_handle
        .wait_with_output()
        .expect("Unexpected error while opening sha256sum stdout");

    let mut hash: String = String::from_utf8_lossy(&sha256sum_handle.stdout).into();
    hash.pop();
    hash.pop();
    hash.pop();
    hash.pop();

    hash
}