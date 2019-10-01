use std::fs::File;
use std::io::prelude::*;

fn main() -> std::io::Result<()> {
  let mut file = File::create("hoge_rs.txt")?;
  for i in 0..1000000 {
    write!(&mut file, "hoge:{}\n",i)?;  
  }
  Ok(())
}