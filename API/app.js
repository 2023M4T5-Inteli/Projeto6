// Importando os módulos necessários
const mqtt = require('mqtt')
const express = require('express')
const bodyParser = require('body-parser')
const sqlite3 = require('sqlite3').verbose()
const path = require('path')

// Inicializando o aplicativo Express
const app = express()
const port = 3000


// Middlewares do Express.js
app.use(bodyParser.urlencoded({extended: false}))
app.use(bodyParser.json())
app.use(express.static(path.join(__dirname, '../views')))

// URL do broker MQTT
const url = 'mqtt://broker.emqx.io:1883'

// Opções de conexão MQTT
const options = {
  clean: true,
  connectTimeout: 4000,
  clientId: 'nodejs_client',
  username: 'emqx_test',
  password: 'emqx_test',
}

// Inicializando o banco de dados
let db = new sqlite3.Database('../data/database.db', (err) => {
  if (err) {
    console.error(err.message)
  } else {
    console.log("Conectado ao banco de dados")
  }
})

// Estabelecendo a conexão MQTT
const client  = mqtt.connect(url, options)

// Ao se conectar ao broker MQTT, se inscreva no tópico
client.on('connect', function () {
  console.log('Conectado ao broker MQTT')
  client.subscribe('meutopico', function (err) {
    if (err) {
      console.error('Falha ao se inscrever no tópico')
    } else {
      console.log('Inscrito no tópico')
    }
  })
})

// Quando uma mensagem é recebida, insira-a no banco de dados
client.on('message', function (topic, message) {
  let sql = `INSERT INTO Roteadores (ID_ROTEADOR) VALUES(${message.toString()})`
  db.run(sql, function(err){
    if (err) {
      console.error(err.message)
    } else {
      console.log("Dado inserido")
    }
  })
})


// Inserir clock e ID no banco de dados via MQTT
client.on('message', function(topic, message){
  var clock = Clock.toString()
  var id = Id.toString()
  console.log(clock)

  let sql = `INSERT INTO Rastreadores (ID_Rastrador,Horario) VALUES (${clock}, ${id})`
  db.run(sql, function(err){
    if (err) {
      console.error(err.message)
    } else {
      console.log("Dado inserido")
    }
  })
})



// Rota para servir o arquivo index.html
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, '../views', 'index.html'))
})

// Rota para registrar um novo roteador
app.post('/cadastrar_roteador', (req, res) => {
  let sql = `INSERT INTO Roteadores (Coordenada_X, Coordenada_Y, ID_roteador) VALUES(${req.body.coordenada_X},${req.body.coordenada_Y},${req.body.ID_roteador})`
  db.run(sql, (err) => {
    if (err) {
      console.error(err.message)
      res.status(500).send('Erro ao cadastrar roteador')
    } else {
      res.sendFile(path.join(__dirname, '../views', 'index.html'))
    }
  })
})

// Rota para registrar um novo rastreador
app.post('/cadastrar_rastreador', (req, res) => {
  let sql = `INSERT INTO Rastreadores (ID_Roteadores, ID_Rastreador, Horario) VALUES(${req.body.id_roteador},${req.body.id_rastreador},${req.body.horario})`
  db.run(sql, (err) => {
    if (err) {
      console.error(err.message)
      res.status(500).send('Erro ao cadastrar rastreador')
    } else {
      res.sendFile(path.join(__dirname, '../views', 'index.html'))
    }
  })
})

// Inicia o servidor Express
app.listen(port, () => {
    console.log(`Servidor rodando em http://localhost:${port}`)
})
