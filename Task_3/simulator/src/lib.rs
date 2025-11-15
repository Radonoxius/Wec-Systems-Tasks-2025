use std::{fs::OpenOptions, io::Read};

pub mod ffi;

const SHADER_PATH: &str = "shaders/solver.cl";

pub fn read_shader() -> Vec<u8> {
    let mut src = Vec::new();
    let mut handle = OpenOptions::new().read(true).open(SHADER_PATH).unwrap();

    handle.read_to_end(&mut src).unwrap();

    src
}