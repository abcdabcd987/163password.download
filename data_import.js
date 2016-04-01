const DBPATH = '/home/abcdabcd987/test.db';
const LOGPATH = 'log.txt';

var fs = require('fs');
var readline = require('readline');
var levelup = require('levelup');

var db = levelup(DBPATH);
var filename = process.argv[2];
var rl = readline.createInterface({ input: fs.createReadStream(filename) });
var log = fs.createWriteStream(LOGPATH, { flags: 'a' });
var emailMatcher = /[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9.]+/;
var count = { line: 0, emptyKey: 0, existKey: 0, success: 0, failure: 0 };

function onLine(line) {
    var email = emailMatcher.exec(line);
    if (!email) return;
    email = email[0];

    db.get(email, function(err, value) {
        ++count.line;
        if (err) {
            if (err.type == 'NotFoundError') {
                value = "";
                ++count.emptyKey;
            } else {
                ++count.existKey;
                log.write(filename + " " + err + "\n");
                return;
            }
        }
        db.put(email, line + "\n" + value, function(err) {
            if (err) {
                ++count.failure;
                log.write(filename + " " + err + "\n");
                return;
            }
            ++count.success;
        })
    })
}

rl.on('line', onLine);

function onExit(err) {
    db.close();
    console.log(filename, count);
    if (err) console.log(err);
}

process.on('exit', onExit);