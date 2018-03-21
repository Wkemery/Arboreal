extern crate clap;
use clap::{App, Arg};
use std::fs::File;
use std::io::prelude::*;


fn main() {
    let  matches = App::new("wtar")
        .version("1.0")
        .author("Wyatt E. <wyatt.k.emery@gmail.com>")
        .about("A small program for creating scripts compatible with the Arboreal Filesystem")
        .arg(Arg::with_name("numfiles")
            .help("The number of files to create on the system")
            .takes_value(true)
            .short("f")
            .long("numfiles")
            .required(true)
        )
        .arg(Arg::with_name("numtags")
            .help("The number of tags to create on the system")
            .takes_value(true)
            .short("t")
            .long("numtags")
            .required(true)
        )
        .arg(Arg::with_name("output")
            .help("The output script file")
            .takes_value(true)
            .short("o")
            .long("output")
            .required(false)
        )
        .arg(Arg::with_name("dictionary")
            .help("A dictionary file to create file and tag names from.")
            .takes_value(true)
            .short("d")
            .long("dict")
            .required(false)
        )
        .arg(Arg::with_name("mintags")
            .help("the minimum number of tags each file can have")
            .takes_value(true)
            .long("mintags")
            .required(false)
        )
        .arg(Arg::with_name("maxtags")
            .help("the maximum number of tags each file can have")
            .takes_value(true)
            .long("maxtags")
            .required(false)
        )
        .get_matches();

    if let Some(dictionary_file) = matches.value_of("dictionary") {
        println!("Dictionary File: {}", dictionary_file);
    }

    let num_files = matches.value_of("numfiles").unwrap();
    println!("Numer of files: {}", num_files);

    let num_tags = matches.value_of("numtags").unwrap();
    println!("Number of tags: {}", num_tags);

    let output_filename = matches.value_of("output").unwrap_or("script.a");
    println!("Output file is: {}", output_filename);

    let tag_range = (matches.value_of("mintags").unwrap_or("1"), matches.value_of("maxtags").unwrap_or("35"));
    println!("Tag range is: {:?}", tag_range);

    /*Create the output file*/
    let mut script_file = File::create(&output_filename).expect("could not create file");
    //script_file.write("new -t".as_bytes());//just seeing how it works

    /*If there is a dictionary file, read in all the entries*/

    /*Write out the create tag commands*/

    /*Write out the create file commands*/

}
