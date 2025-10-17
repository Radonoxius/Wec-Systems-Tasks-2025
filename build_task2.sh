#! /bin/bash

cd Task_2/shracker

cargo r --release --bin shracker_serial

cargo r --release --bin shracker

cargo clean
cd ../..