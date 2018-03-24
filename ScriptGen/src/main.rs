extern crate clap;
use clap::{App, Arg};
use std::fs::File;
use std::io::prelude::*;
extern crate rand;
use rand::{thread_rng, Rng};
use std::collections::HashSet;
use std::io::{BufReader, BufWriter};

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
        .arg(Arg::with_name("FileDupProb")
            .help("The probability as 1/n of duplicating a file. Then the probability of duplicating it twice is 1/n^2 \
                    and so on")
            .takes_value(true)
            .long("filedup")
            .required(false)
        )
        .arg(Arg::with_name("FileMult")
            .help("The frequency as 1/n of files that will be duplicated a number of times according to the range \
                  specified from MultMin and MultMax")
            .takes_value(true)
            .long("filemult")
            .required(false)
        )
        .arg(Arg::with_name("MultMin")
            .help("The lower end of the range used by FileMult")
            .takes_value(true)
            .long("multmin")
            .required(false)
        )
        .arg(Arg::with_name("MultMax")
            .help("the upper end of the range used by FileMult")
            .takes_value(true)
            .long("multmax")
            .required(false)
        )
        .get_matches();


    /*Set up Args*/
    let output_filename = matches.value_of("output").unwrap_or("script.ab");

    let mut script_atts = ScriptAttributes::new(&matches);
    
    /*Create the output file*/
    let mut script_file = File::create(&output_filename).expect("could not create file");

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
    
    /*Write out end*/
    script_file.write("end".as_bytes()).expect("Could not write to file");
}

struct ScriptAttributes {
    names : Vec<String>,
    num_tags: u32,
    num_files: u32,
    tag_range: (u32, u32),
    file_dup_prob: u32,
    file_mult_freq: u32,
    file_mult_range: (u32, u32),
}

impl ScriptAttributes {
    fn new(matches: &clap::ArgMatches) -> ScriptAttributes{
        let num_files = matches.value_of("numfiles").unwrap().parse::<u32>().expect("Invalid number of\
                    Files specified");

        let num_tags = matches.value_of("numtags").unwrap().parse::<u32>().expect("Invalid number of\
                    Tags specified");

        let tag_range = (matches.value_of("mintags").unwrap_or("1").parse::<u32>().expect("Invalid minimum tag number specified"),
            matches.value_of("maxtags").unwrap_or("35").parse::<u32>().expect("Invalid maximum tag number specified"));
    
        let file_dup_prob = matches.value_of("FileDupProb").unwrap_or("10").parse::<u32>().expect("Invalid file duplication probability specified");
    
        let file_mult_freq = matches.value_of("FileMult").unwrap_or("10000").parse::<u32>().expect("Invalid multiple file name frequency specified");
    
        let file_mult_range = (matches.value_of("MultMin").unwrap_or("10").parse::<u32>().expect("Invalid lower bound for FileMult specified"),
            matches.value_of("MultMax").unwrap_or("100").parse::<u32>().expect("Invalid upper bound for FileMult specified"));
            
        ScriptAttributes{names: Vec::new(), num_tags: num_tags, num_files: num_files, tag_range: tag_range, 
                        file_dup_prob: file_dup_prob, file_mult_freq: file_mult_freq, file_mult_range: file_mult_range}
    }

    fn read_dict(&mut self, dict_file : File){
        let reader = BufReader::new(&dict_file);
        let mut i = 0;
        
        for line in reader.lines() {
            if i > (self.num_tags + self.num_files){ break; }
            self.names.push(line.expect("Unable to read dictionary file"));
            i += 1;
        }
    }

}

fn write_tag_commands(script_atts: &mut ScriptAttributes, script_file: &mut File) -> Vec<String> {
    let mut writer = BufWriter::new(script_file);
    let mut ret = Vec::new();
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

        writer.write(out_line.as_bytes()).expect("Unable to write to file");
    }
    ret
}

fn write_file_commands(script_atts: &mut ScriptAttributes, script_file: &mut File, tag_list: Vec<String>) {
    let mut writer = BufWriter::new(script_file);
    let mut file_mult_counter = 0;
    let mut file_mult_num = 0;
    let mut start_mult = false;
    let mut file_mult_file_count = 0;
    let mut k = 0;
    
    for _ in 0..script_atts.num_files {
        let mut tag_set = HashSet::new();
        let num_tags = thread_rng().gen_range(script_atts.tag_range.0, script_atts.tag_range.1);
        
        for _ in 0..num_tags {
            if let Some(tag) = tag_list.get(thread_rng().gen_range(0, tag_list.len())) {
                tag_set.insert(tag.as_str());
            }
        }
        
        let mut tags = String::from("/");
        for tag in tag_set {
            tags.push_str(tag);
            tags.push_str("/");
        }
                
        let out_line =
            if let Some(name) = script_atts.names.last() { format!("new {}file_{}\n", tags, name) }
            else { format!("new {}file_{}\n",tags, k) };
        
        if file_mult_counter == script_atts.file_mult_freq { 
            start_mult = true; 
            file_mult_counter = 0;
            file_mult_num = thread_rng().gen_range(script_atts.file_mult_range.0, script_atts.file_mult_range.1 + 1);
        }

        let to_pop =
            if start_mult {file_mult_file_count+= 1; false}
            else if thread_rng().gen_range(0, script_atts.file_dup_prob + 1) == script_atts.file_dup_prob {false}
            else {file_mult_counter+= 1; true};
        
        if file_mult_file_count == file_mult_num {
            start_mult = false;
        }
        
        if to_pop {
            script_atts.names.pop();
            k+= 1;
        }
        writer.write(out_line.as_bytes()).expect("Unable to write to file");
    }
}
