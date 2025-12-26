struct Post {
  id : Int
  name : String
  message : String
}

fn get_db() -> @sqlite3.Connection {
  @sqlite3.Connection::open("bbs.db") catch { _ => abort("Failed to open database") }
}

fn init_db(db : @sqlite3.Connection) -> Unit {
  let sql =
    #| CREATE TABLE IF NOT EXISTS posts (
    #|   id INTEGER PRIMARY KEY AUTOINCREMENT,
    #|   name BLOB NOT NULL,
    #|   message BLOB NOT NULL
    #| );
  let stmt = db.prepare(sql) catch { _ => return }
  stmt.step_once() catch { _ => () }
  stmt.finalize() catch { _ => () }
}

fn add_post(db : @sqlite3.Connection, name : String, message : String) -> Unit {
  let sql =
    #| INSERT INTO posts (name, message) VALUES (?, ?);
  let stmt = db.prepare(sql) catch { _ => return }
  stmt.bind_string_as_blob(index=1, val=name) catch { _ => () }
  stmt.bind_string_as_blob(index=2, val=message) catch { _ => () }
  stmt.step_once() catch { _ => () }
  stmt.finalize() catch { _ => () }
}

fn get_posts(db : @sqlite3.Connection) -> Array[Post] {
  let posts : Array[Post] = []
  let sql =
    #| SELECT id, name, message FROM posts ORDER BY id DESC;
  let stmt = db.prepare(sql) catch { _ => return posts }
  while {
    let has_row = stmt.step() catch { _ => false }
    if not(has_row) {
      false
    } else {
      let id : Int = stmt.column(index=0) catch { _ => 0 }
      let name : String = stmt.column_blob_as_string(index=1) catch { _ => "" }
      let message : String = stmt.column_blob_as_string(index=2) catch { _ => "" }
      posts.push({ id, name, message })
      true
    }
  } {
    continue
  }
  stmt.finalize() catch { _ => () }
  posts
}

fn url_decode(s : String) -> String {
  let buf = @buffer.new()
  let len = s.length()
  let mut i = 0
  while i < len {
    let c = s[i]
    if c == '+' {
      buf.write_char(' ')
      i = i + 1
    } else if c == '%' && i + 2 < len {
      let high = hex_to_int(s[i + 1])
      let low = hex_to_int(s[i + 2])
      if high >= 0 && low >= 0 {
        let code = high * 16 + low
        buf.write_char(code.unsafe_to_char())
      }
      i = i + 3
    } else {
      match c.to_char() {
        Some(ch) => buf.write_char(ch)
        None => ()
      }
      i = i + 1
    }
  }
  buf.to_string()
}

fn hex_to_int(c : UInt16) -> Int {
  if c >= '0' && c <= '9' {
    c.to_int() - '0'.to_int()
  } else if c >= 'a' && c <= 'f' {
    c.to_int() - 'a'.to_int() + 10
  } else if c >= 'A' && c <= 'F' {
    c.to_int() - 'A'.to_int() + 10
  } else {
    -1
  }
}

fn parse_form_data(body : String) -> Map[String, String] {
  let result : Map[String, String] = {}
  let pairs = body.split("&")
  for pair in pairs {
    let kv : Array[String] = pair.split("=").map(fn(x) { x.to_string() }).collect()
    if kv.length() >= 2 {
      let key = url_decode(kv[0])
      let value = url_decode(kv[1])
      result[key] = value
    }
  }
  result
}

fn render_html(db : @sqlite3.Connection) -> String {
  let buf = @buffer.new()
  let header =
    #| <!DOCTYPE html>
    #| <html>
    #| <head>
    #|   <meta charset="UTF-8">
    #|   <title>BBS</title>
    #| </head>
    #| <body>
    #|   <h1>Simple BBS (SQLite)</h1>
    #|   <form method="POST" action="/post">
    #|     <input type="text" name="name" placeholder="Name" required><br><br>
    #|     <textarea name="message" placeholder="Message" required></textarea><br><br>
    #|     <button type="submit">Post</button>
    #|   </form>
    #|   <hr>
    #|   <h2>Posts</h2>
  buf.write_string(header)
  let posts = get_posts(db)
  if posts.length() == 0 {
    buf.write_string("<p>No posts yet.</p>")
  } else {
    for post in posts {
      buf.write_string("<div style=\"border:1px solid #ccc;padding:10px;margin:10px 0\">")
      buf.write_string("<small>#")
      buf.write_string(post.id.to_string())
      buf.write_string("</small> ")
      buf.write_string("<strong>")
      buf.write_string(post.name)
      buf.write_string("</strong>")
      buf.write_string("<p>")
      buf.write_string(post.message)
      buf.write_string("</p>")
      buf.write_string("</div>")
    }
  }
  let footer =
    #| </body>
    #| </html>
  buf.write_string(footer)
  buf.to_string()
}

fn main {
  let db = get_db()
  init_db(db)
  let app = @mocket.new()

  app.get("/", fn(_event) {
    let res = @mocket.HttpResponse::new(@mocket.OK)
    res.headers["Content-Type"] = "text/html; charset=utf-8"
    res.body(render_html(db))
  })

  app.post("/post", fn(event) {
    let body : String = event.req.body() catch { _ => "" }
    let form = parse_form_data(body)
    let name = form.get("name").unwrap_or("")
    let message = form.get("message").unwrap_or("")
    if name != "" && message != "" {
      add_post(db, name, message)
    }
    let res = @mocket.HttpResponse::new(@mocket.Found)
    res.headers["Location"] = "/"
    res
  })

  println("Server running at http://localhost:4000")
  app.serve(port=4000)
}
