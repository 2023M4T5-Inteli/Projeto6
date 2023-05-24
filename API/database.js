
const sqlite3 = require('sqlite3').verbose();

let db = new sqlite3.Database('database.db', (err) => {
  if (err) {
    return console.error(err.message);
  }
  console.log('Conectado ao banco de dados SQLite.');
});

db.serialize(() => {
  db.run(`CREATE TABLE IF NOT EXISTS "Roteadores" (
            "Coordenada_X" INTEGER,
            "Coordenada_Y" INTEGER,
            "ID_roteador" INTEGER,
            PRIMARY KEY("ID_roteador")
          )`, (err) => {
    if (err) {
      console.error(err.message);
    }
  });

  db.run(`CREATE TABLE IF NOT EXISTS "Rastreadores" (
            "ID_Roteadores" INTEGER,
            "ID_Rastreador" INTEGER,
            "Horario" INTEGER,
            PRIMARY KEY("ID_Rastreador"),
            FOREIGN KEY("ID_Roteadores") REFERENCES "Roteadores"("ID_roteador")
          )`, (err) => {
    if (err) {
      console.error(err.message);
    }
  });
});

db.close((err) => {
  if (err) {
    console.error(err.message);
  }
  console.log('Fechou a conexão com o banco de dados SQLite.');
});