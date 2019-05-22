var express = require("express")
var app = express()

var multipart = require('connect-multiparty');
var multipartMiddleware = multipart();

app.post('/', multipartMiddleware, function(req, res, next) {
    // 业务逻辑
    console.log(req.body);
    res.end("hello post")
});
app.get('/', function(req, res) {
    console.log("hello get")
    res.end("hello get")
})
app.listen(3000)

