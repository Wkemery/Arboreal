use std::fs::File;
use std::io::prelude::*;
extern crate rand;
use rand::{thread_rng, Rng};
use std::io::{BufWriter};


fn main() {

    let output_filename = String::from("big_scripts.txt");
    let mut script_file = File::create(&output_filename).expect("could not create file");
    let mut writer = BufWriter::new(&mut script_file);
    for i in 23..35{
        let out_line = format!("cargo run -- -f {} -t {} --maxtags 50 -o script{}.ab -d words_alpha.txt --filemult {} --multmin 25 --multmax 750\n",
                    thread_rng().gen_range(100000, 1000000), thread_rng().gen_range(150, 650), i, thread_rng().gen_range(200, 1000));
        writer.write(out_line.as_bytes()).expect("Could not write to File");
    }

}
