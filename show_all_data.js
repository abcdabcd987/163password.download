#!/usr/bin/env node

const DBPATH = '/home/abcdabcd987/test.db';
var levelup = require('levelup');
var db = levelup(DBPATH);
db.createReadStream()
  .on('data', function (data) {
     console.log(data.key, '=', data.value)
   });