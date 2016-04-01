const DBPATH = '/home/abcdabcd987/test.db';
const LOGPATH = 'log.txt';
const BATCHSIZE = 4*1024*1024;

var fs = require('fs');
var readline = require('readline');
var levelup = require('levelup');

var db = levelup(DBPATH);
var filename = process.argv[2];
var rl = readline.createInterface({ input: fs.createReadStream(filename) });
var log = fs.createWriteStream(LOGPATH, { flags: 'a' });
var emailMatcher = /[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9.]+/;
var count = { line: 0, valid: 0, write: 0, batchFailure: 0 };
var batch = db.batch();

function batchWrite() {
    ++count.write;
    batch.write(function(err) {
        batch.clear();
        if (err) {
            ++count.batchFailure;
            log.write(filename + " " + err + "\n");
        }
    })
}

function onLine(line) {
    ++count.line;
    var email = emailMatcher.exec(line);
    if (!email) return;
    email = email[0];
    ++count.valid;

    batch.put(email, line);
    if (batch.length < BATCHSIZE) return;
    batchWrite();
}

rl.on('line', onLine);

function onExit(err) {
    batchWrite();
    db.close();
    console.log(filename, count);
    if (err) console.log(err);
}

process.on('exit', onExit);