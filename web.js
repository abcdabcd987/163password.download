#!/usr/bin/env node

const PORT = 10086;
const DBPATH = '/home/abcdabcd987/test.db';


var fs = require('fs');
var path = require('path');
var http = require('http');
var levelup = require('levelup');
var db = levelup(DBPATH);

var indexHTMLPath = path.join(__dirname, 'index.html');
var indexHTMLContent = fs.readFileSync(indexHTMLPath);

function handleRequest(req, res) {
    res.setHeader('Cache-Control', 'public,max-age=31536000');

    if (req.url == '/') {
        res.end(indexHTMLContent);
    } else {
        var email = req.url.replace('/?e=', '').replace('%40', '@');
        db.get(email, function(err, value) {
            if (err) value = '未找到';
            res.setHeader('Content-Type', 'text/plain');
            res.end(value);
        })
    }
}

var server = http.createServer(handleRequest);

server.listen(PORT, function(){
    console.log("Server listening on: http://localhost:%s", PORT);
});