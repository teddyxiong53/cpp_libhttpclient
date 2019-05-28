var express = require("express")
var app = express()

var bodyParser = require("body-parser")

app.use(bodyParser.urlencoded({extended:false}))

app.post('/', function(req, res, next) {
    // 业务逻辑
    console.log(req.body);
    res.end("hello post")
});
app.get('/', function(req, res) {
    console.log("hello get")
    console.log(req.query)
    res.end("hello get")
})
app.listen(3000)

