use std::fs::File;
use std::io::{BufWriter,Write};

fn main() -> std::io::Result<()> {
  let file = File::create("hoge_rs.txt")?;
  let mut b = BufWriter::new(file);
  for i in 0..1000000 {
    let s = format!("hoge:{}\n",i);  
    b.write( & s.into_bytes() )?;
  }
  Ok(())
}