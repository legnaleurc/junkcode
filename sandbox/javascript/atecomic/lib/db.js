'use strict';

var sqlite3 = require('sqlite3').verbose();

var gDB = null;


function Dababase (path) {
  var _ = this._ = {};
  _.db = new sqlite3.Database(path);
  _.waitingRefresh = null;
  createTable(_.db);
  return this;
}

Dababase.prototype.addRefreshTasks = function addRefreshTasks (comic_list) {
  var db = this._.db;

  db.serialize(() => {
    var statement = db.prepare('INSERT OR IGNORE INTO `refresh_tasks` (`comic_id`, `url`, `dirty`) VALUES (?, ?, ?);');
    comic_list.forEach((comic) => {
      statement.run(comic.id, comic.url, 1);
    });
    statement.finalize();
  });

  return Promise.resolve(this);
};


// see notifyDirtyRefresh
Database.prototype.getDirtyRefreshTasks = function getDirtyRefreshTasks () {
  var _ = this._;

  if (_.waitingRefresh) {
    throw new Error('reentrancy is not allowed');
  }

  return new Promise((resolve, reject) => {
    _.waitingRefresh = resolve;

    notifyDirtyRefresh(_);
  });
};


// see Database.prototype.getDirtyRefreshTasks
function notifyDirtyRefresh (_) {
  if (!_.waitingRefresh) {
    return Promise.resolve();
  }

  return getDirtyRefreshTasks_(_.db).then((tasks) => {
    if (tasks.length > 0) {
      _.waitingRefresh(tasks);
    }
  });
};


function getDirtyRefreshTasks_ (db) {
  var tasks = [];
  db.serialize(() => {
    var statement = db.prepare('SELECT `comic_id`, `url` FROM `refresh_tasks` WHERE `dirty` = 1;');
    statement.all((e, rows) => {
      rows = rows.map((row) => {
        return {
          id: row.comic_id,
          url: row.url,
        };
      });
      tasks.concat(rows);
    });
    statement.finalize();
  });
  return Promise.resolve(tasks);
}


function createTable (rawDB) {
  rawDB.serialize(() => {
    rawDB.run(`CREATE TABLE IF NOT EXISTS \`refresh_tasks\` (
      \`id\` INTEGER PRIMARY KEY AUTOINCREMENT,
      \`comic_id\` INTEGER UNIQUE NOT NULL,
      \`url\` TEXT NOT NULL,
      \`dirty\` INTEGER NOT NULL
    );`);
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
