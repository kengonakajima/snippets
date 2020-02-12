use std::fs::File;
use std::io::prelude::*;

fn main() -> std::io::Result<()> {
  let mut file = File::create("hoge_rs.txt")?;
  for i in 0..1000000 {
    let s = format!("hoge:{}\n",i);  
    file.write( & s.into_bytes() )?;
//    file.flush()?;
  }
  Ok(())
}