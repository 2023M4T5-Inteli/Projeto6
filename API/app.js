const express = require('express')
const bodyParser = require('body-parser')
const sqlite3 = require('sqlite3').verbose()
const path = require('path')
const app = express()
const port = 3000


app.use(bodyParser.urlencoded({extended: false}))
app.use(bodyParser.json())
app.use(express.static(path.join(__dirname, '../views')))

let db = new sqlite3.Database('../data/database.db', (err) => {
    if (err == true) {
        console.log(err.message)
    }
    console.log("Conectado à base de dados")
})


app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, '../views', 'index.html'))
})


app.post('/cadastrar_roteador',(req,res)=>{
    //const {coordenada_X, coordenada_Y, id_roteador} = req.body
    let coord_x = req.body.coordenada_X
    let coord_y = req.body.coordenada_Y
    let id = req.body.ID_roteador
    let sql = `INSERT INTO Roteadores (Coordenada_X, Coordenada_Y, ID_roteador) VALUES(${coord_x},${coord_y},${id})`

    db.run(sql,(err)=>{
        if (err){
            console.log(err.message)
            res.status(500).send('Erro ao cadastrar roteador')
        }
        res.sendFile(path.join(__dirname, '../views', 'index.html'))
    })
})


app.post('/cadastrar_rastreador',(req,res)=>{
    let id_roteador = req.body.id_roteador
    let id_rastreador = req.body.id_rastreador
    let horario = req.body.horario
    let sql = `INSERT INTO Rastreadores (ID_Roteadores, ID_Rastreador, Horario) VALUES(${id_roteador},${id_rastreador},${horario})`

    db.run(sql,(err)=>{
        if (err){
            console.log(err.message)
            res.status(500).send('Erro ao cadastrar rasterador')
        }
        res.sendFile(path.join(__dirname, '../views', 'index.html'))
    })
})


app.listen(port,()=>{
    console.log(`Servidor rodando em http://localhost:${port}`)
})