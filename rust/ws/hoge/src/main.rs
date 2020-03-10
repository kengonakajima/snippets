extern crate ws;
use ws::listen;
fn main() {
    println!("hoge");
    if let Err(error) = listen("127.0.0.1:3012", |out| {
        move |msg| {
            let response: String = format!("{} - {}", msg, "Im server.".to_string());
            println!("{}", response);
            out.send(response)  // ここにセミコロンつけたらわけわからんエラーがすんごい出る。
        }
    }) {
        println!("Failed to create WebSocket due to {:?}", error);
    }
}

