'use strict';

var sqlite3 = require('sqlite3').verbose();

var gDB = null;


function Dababase (path) {
  var _ = this._ = {};
  _.db = new sqlite3.Database(path);
  createTable(_.db);
  return this;
}

Dababase.prototype.addRefreshTasks = function addRefreshTasks (comic_list) {
  var db = this._.db;

  db.serialize(() => {
    var statement = db.prepare('INSERT INTO `refresh_tasks` (`comic_id`, `url`) VALUES (?, ?)');
    comic_list.forEach((comic) => {
      statement.run(comic.id, comic.url);
    });
    statement.finalize();
  });

  return Promise.resolve(this);
};


function createTable (rawDB) {
  rawDB.serialize(() => {
    rawDB.run(`CREATE TABLE IF NOT EXISTS \`refresh_tasks\` (
      \`id\` INTEGER PRIMARY KEY AUTOINCREMENT,
      \`comic_id\` INTEGER UNIQUE NOT NULL,
      \`url\` TEXT NOT NULL
    )`);
  });
}


function getInstance () {
  if (!gDB) {
    gDB = new Dababase('/tmp/atecomic.sqlite');
  }
  return gDB;
}


function main () {
  var db = getInstance();

  return 0;
}


if (!module.parent) {
  return main();
}


module.exports = {
  getInstance: getInstance,
};
