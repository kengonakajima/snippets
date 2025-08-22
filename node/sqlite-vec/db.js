import Database from 'better-sqlite3';
import * as sqlite_vec from 'sqlite-vec';

const db = new Database('./vector_demo.db');
sqlite_vec.load(db);

export function initDatabase() {
  db.exec(`
    CREATE TABLE IF NOT EXISTS documents (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      text TEXT NOT NULL,
      embedding BLOB NOT NULL,
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP
    )
  `);

  db.exec(`
    CREATE VIRTUAL TABLE IF NOT EXISTS vec_documents USING vec0(
      embedding float[1536]
    )
  `);

  console.log('Database initialized successfully');
}

export function insertDocument(text, embedding) {
  const insertDoc = db.prepare(`
    INSERT INTO documents (text, embedding) VALUES (?, ?)
  `);
  
  const result = insertDoc.run(text, Buffer.from(new Float32Array(embedding).buffer));
  const docId = Number(result.lastInsertRowid);

  const insertVec = db.prepare(`
    INSERT INTO vec_documents (embedding) VALUES (?)
  `);
  const vecResult = insertVec.run(new Float32Array(embedding));

  return docId;
}

export function insertDocumentsBatch(documents) {
  const insertDoc = db.prepare(`
    INSERT INTO documents (text, embedding) VALUES (?, ?)
  `);
  
  const insertVec = db.prepare(`
    INSERT INTO vec_documents (embedding) VALUES (?)
  `);

  const insertMany = db.transaction((docs) => {
    const ids = [];
    for (const doc of docs) {
      const result = insertDoc.run(doc.text, Buffer.from(new Float32Array(doc.embedding).buffer));
      const docId = Number(result.lastInsertRowid);
      insertVec.run(new Float32Array(doc.embedding));
      ids.push(docId);
    }
    return ids;
  });

  return insertMany(documents);
}

export function searchSimilar(queryEmbedding, limit = 5) {
  const query = db.prepare(`
    SELECT 
      d.id,
      d.text,
      d.created_at,
      vec_distance_cosine(v.embedding, ?) as distance
    FROM vec_documents v
    JOIN documents d ON v.rowid = d.id
    ORDER BY distance
    LIMIT ?
  `);

  return query.all(new Float32Array(queryEmbedding), limit);
}

export function listDocuments() {
  const query = db.prepare(`
    SELECT id, text, created_at FROM documents ORDER BY created_at DESC
  `);
  return query.all();
}

export function deleteDocument(id) {
  db.prepare('DELETE FROM vec_documents WHERE rowid = ?').run(id);
  const result = db.prepare('DELETE FROM documents WHERE id = ?').run(id);
  return result.changes > 0;
}

export function clearAllDocuments() {
  db.prepare('DELETE FROM vec_documents').run();
  db.prepare('DELETE FROM documents').run();
}

export default db;