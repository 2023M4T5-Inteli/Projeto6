const mqtt = require('mqtt')
const express = require('express')
const bodyParser = require('body-parser')
const sqlite3 = require('sqlite3').verbose()
const path = require('path')
const app = express()
const port = 3000

const url = 'mqtt://broker.emqx.io:1883'

const options = {
  clean: true,
  connectTimeout: 4000,
  clientId: 'nodejs_client',
  username: 'emqx_test',
  password: 'emqx_test',
}


let db = new sqlite3.Database('../data/database.db', (err) => {
  if (err == true) {
      console.log(err.message)
  }
  console.log("Conectado à base de dados")
})



const client  = mqtt.connect(url, options)

client.on('connect', function () {
  console.log('Conectado')
  client.subscribe('meutopico', function (err) {
    if (err) {
      console.error('Falha ao inscrever no topico')
    } else {
      console.log('Inscrito no topico')
    }
  })
})


client.on('message', function (topic, message) {
  var mensagem = message.toString()
  console.log(mensagem)
  //client.end()
  let sql = `INSERT INTO Roteadores (ID_ROTEADOR) VALUES(${mensagem})`
  db.run(sql, function(err){
    if (err){
      console.log(err.message)
    }else {
      console.log("Dado inserido")
    }
  })
})


//db.run(`DELETE FROM Roteadores`, (err) => {
//  if (err) {
//    console.error(err.message);
//  } else {
//    console.log("Todos os dados apagados da tabela Roteadores.");
//  }
//});
//
//db.close();
//