extern crate clap;
use clap::{App, Arg};
use std::fs::File;
use std::io::prelude::*;
extern crate rand;
use rand::Rng;


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



    let num_files = matches.value_of("numfiles").unwrap().parse::<u32>().expect("Invalid number of\
                    Files specified");
    println!("Numer of files: {}", num_files);

    let num_tags = matches.value_of("numtags").unwrap().parse::<u32>().expect("Invalid number of\
                    Tags specified");
    println!("Number of tags: {}", num_tags);

    let output_filename = matches.value_of("output").unwrap_or("script.ab");
    println!("Output file is: {}", output_filename);

    let tag_range = (matches.value_of("mintags").unwrap_or("1").parse::<u32>().expect("Invalid minimum tag number specified"),
        matches.value_of("maxtags").unwrap_or("35").parse::<u32>().expect("Invalid maximum tag number specified"));
    println!("Tag range is: {:?}", tag_range);

    /*Create the output file*/
    let mut script_file = File::create(&output_filename).expect("could not create file");

    let mut script_atts = ScriptAttributes{names: Vec::new(), num_tags: num_tags,
        num_files: num_files, tag_range: tag_range};
    /*If there is a dictionary file, read in all the entries*/
    if let Some(dictionary_file) = matches.value_of("dictionary") {
        println!("Dictionary File: {}", dictionary_file);
        let mut dict_file = File::open(dictionary_file).expect("Could not open dictionary file");
        script_atts.read_dict(dict_file);
    }

    /*Write out the create tag commands*/
    let tag_list = write_tag_commands(&mut script_atts, &mut script_file);
    /*Write out the create file commands*/
    write_file_commands(&mut script_atts, &mut script_file, tag_list);
}

struct ScriptAttributes {
    names : Vec<String>,
    num_tags: u32,
    num_files: u32,
    tag_range: (u32, u32),
}

impl ScriptAttributes {
    fn read_dict(&mut self, dict_file : File){
        //TODO
        self.names.push(String::from("test"));
    }

}

fn write_tag_commands(script_atts: &mut ScriptAttributes, script_file: &mut File) -> Vec<String> {
    let mut ret = vec![String::from("default")];
    for i in 0..script_atts.num_tags {
        let out_line =
            if let Some(name) = script_atts.names.pop() {
                ret.push(format!("tag_{}", name));
                format!("new -t [tag_{}]\n", name)
            }
            else {
                ret.push(format!("tag_{}", i));
                format!("new -t [tag_{}]\n", i)
            };

        script_file.write(out_line.as_bytes()).expect("Unable to write to file");
    }
    ret
}

fn write_file_commands(script_atts: &mut ScriptAttributes, script_file: &mut File, tag_list: Vec<String>) {
    for i in 0..script_atts.num_files {
        let out_line =
            if let Some(name) = script_atts.names.pop() {
                name
            }
            else {
                let mut tags = String::from("/");
                let num_tags = rand::thread_rng().gen_range(script_atts.tag_range.0, script_atts.tag_range.1);
                for i in 0..num_tags {
                    tags = tags + tag_list[rand::thread_rng().gen_range(0,tag_list.len() - 1)].as_str();
                }
                String::from("rest")
            };
            script_file.write(out_line.as_bytes()).expect("Unable to write to file");
    }
}
