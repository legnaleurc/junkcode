'use strict';

var co = require('co');
var sqlite3 = require('co-sqlite3');

var asyncio = require('./asyncio.js');

var gDB = null;


function Database (rawDB) {
  var _ = this._ = {};
  _.waitingRefresh = null;
  _.db = rawDB;
}


Database.prototype.addRefreshTasks = function addRefreshTasks (comicList) {
  var db = this._.db;

  return co(function * () {
    var statement = yield db.prepare('INSERT OR IGNORE INTO `refresh_tasks` (`comic_id`, `url`, `dirty`) VALUES (?, ?, ?);');
    yield asyncio.forEach(comicList, function * (comic) {
      yield statement.run(comic.id, comic.url, 1);
    });
    statement.finalize();
  });
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


Database.prototype.updateComic = function updateComic (comic) {
  var db = this._.db;

  return co(function * () {
    var statement = yield db.prepare('SELECT `id` FROM `comics` WHERE `comic_id` = ?;');
    var rows = yield statement.all(comic.id);
    if (rows.length === 0) {
      statement = yield db.prepare('INSERT INTO `comics` (`comic_id`, `title`, `author`, `mtime`, `cover_url`, `url`, `brief`) VALUES (?, ?, ?, ?, ?, ?, ?);');
      statement = yield statement.run(comic.id, comic.title, comic.author, comic.mtime.getTime(), comic.coverURL, comic.url, comic.brief);
      var comicID = statement.lastID;
      console.info('inserted comic id', comicID);
    } else {
      statement = yield db.prepare('UPDATE `comics` SET `title` = ?, `author` = ?, `mtime` = ?, `cover_url` = ?, `url` = ?, `brief` = ? WHERE `comic_id` = ?;');
      statement = yield statement.run(comic.title, comic.author, comic.mtime.getTime(), comic.coverURL, comic.url, comic.brief, comic.id);
      var comicID = rows[0];
      console.info('found comic id', comicID);
    }

    // TODO flush existing episodes? when?
    // TODO deal about episode's mtime

    yield asyncio.forEach(comic.episodes, function * (episode) {
      var statement = yield db.prepare('SELECT `id` FROM `episodes` WHERE `comic_id` = ? AND `episode_id` = ?;');
      var rows = yield statement.all(comicID, episode.id);
      if (rows.length === 0) {
        statement = yield db.prepare('INSERT INTO `episodes` (`comic_id`, `episode_id`, `title`, `mtime`, `volume`, `chapter`, `url`) VALUES (?, ?, ?, ?, ?, ?, ?);');
        statement = yield statement.run(comicID, episode.id, episode.title, 0, episode.isVolume, episode.isChapter, episode.url);
        var episodeID = statement.lastID;
        console.info('inserted episode id', episodeID);

        yield asyncio.forEach(episode.pages, function * (pageURL) {
          statement = yield db.prepare('INSERT INTO `pages` (`episode_id`, `url`) VALUES (?, ?);');
          statement = yield statement.run(episodeID, pageURL);
          console.info('inserted page id', statement.lastID);
        });
      } else {
        var episodeID = rows[0];
        console.info('found episode id', episodeID);
      }
    });

    statement.finalize();
  });
};


Database.prototype.clearComic = function clearComic (comicID) {
  var db = this._.db;

  return co(function * () {
    var statement = yield db.prepare('UPDATE `refresh_tasks` SET `dirty` = 0 WHERE `comic_id` = ?;');
    statement = yield statement.run(comicID);
    statement.finalize();
    console.info('cleansed comic id', comicID);
  });
};


Database.prototype.getLatestComics = function getLatestComics (offset, length) {
  var db = this._.db;
  offset = typeof offset === 'undefined' ? 0 : offset;
  length = typeof length === 'undefined' ? 0 : length;

  return co(function * () {
    if (length <= 0) {
      var statement = yield db.prepare('SELECT `id`, `title`, `cover_url`  FROM `comics` ORDER BY `mtime` DESC LIMIT -1 OFFSET ?;');
      var rows = yield statement.all(Math.max(offset, 0));
    } else {
      var statement = yield db.prepare('SELECT `id`, `title`, `cover_url`  FROM `comics` ORDER BY `mtime` DESC LIMIT ? OFFSET ?;');
      var rows = yield statement.all(length, offset);
    }
    statement.finalize();
    return rows;
  });
};


Database.prototype.getComics = function getComics (offset, length) {
  var db = this._.db;
  offset = typeof offset === 'undefined' ? 0 : offset;
  length = typeof length === 'undefined' ? 0 : length;

  return co(function * () {
    if (length <= 0) {
      var statement = yield db.prepare('SELECT `id`, `title`, `cover_url`, `author`, `mtime`, `cover_url`, `brief` FROM `comics` LIMIT -1 OFFSET ?;');
      var rows = yield statement.all(Math.max(offset, 0));
    } else {
      var statement = yield db.prepare('SELECT `id`, `title`, `cover_url`, `author`, `mtime`, `cover_url`, `brief` FROM `comics` LIMIT ? OFFSET ?;');
      var rows = yield statement.all(offset, length);
    }
    statement.finalize();
    return rows;
  });
};


Database.prototype.getComic = function getComic (comicID) {
  var db = this._.db;

  return co(function * () {
    var statement = yield db.prepare('SELECT `id`, `title`, `cover_url`, `author`, `mtime`, `cover_url`, `brief` FROM `comics` WHERE `id` = ?;');
    var rows = yield statement.all(comicID);
    statement.finalize();
    if (rows.length === 0) {
      return null;
    } else {
      return rows[0];
    }
  });
};


Database.prototype.getEpisodes = function getEpisodes (comicID) {
  var db = this._.db;

  return co(function * () {
    var statement = yield db.prepare('SELECT `id`, `title`, `volume`, `chapter` FROM `episodes` WHERE `comic_id` = ?;');
    var rows = yield statement.all(comicID);
    statement.finalize();
    return rows.map((row) => {
      return {
        id: row.id,
        title: row.title,
        volume: !!row.volume,
        chapter: !!row.chapter,
      };
    });
  });
};


Database.prototype.getPages = function getPages (episodeID) {
  var db = this._.db;

  return co(function * () {
    var statement = yield db.prepare('SELECT `url` FROM `pages` WHERE `episode_id` = ?;');
    var rows = yield statement.all(episodeID);
    statement.finalize();
    return rows.map((row) => {
      return row.url;
    });
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
  return co(function * () {
    var statement = yield db.prepare('SELECT `comic_id`, `url` FROM `refresh_tasks` WHERE `dirty` = 1;');
    var rows = yield statement.all();
    statement.finalize();
    return rows.map((row) => {
      return {
        id: row.comic_id,
        url: row.url,
      };
    });
  });
}


function * createTable (rawDB) {
  yield rawDB.run(`CREATE TABLE IF NOT EXISTS \`refresh_tasks\` (
    \`id\` INTEGER PRIMARY KEY AUTOINCREMENT,
    \`comic_id\` INTEGER UNIQUE NOT NULL,
    \`url\` TEXT NOT NULL,
    \`dirty\` INTEGER NOT NULL
  );`);

  yield rawDB.run(`CREATE TABLE IF NOT EXISTS \`comics\` (
    \`id\` INTEGER PRIMARY KEY AUTOINCREMENT,
    \`comic_id\` INTEGER UNIQUE NOT NULL,
    \`title\` TEXT NOT NULL,
    \`author\` TEXT NOT NULL,
    \`mtime\` INTEGER NOT NULL,
    \`cover_url\` TEXT NOT NULL,
    \`url\` TEXT NOT NULL,
    \`brief\` TEXT NOT NULL
  );`);

  yield rawDB.run(`CREATE TABLE IF NOT EXISTS \`episodes\` (
    \`id\` INTEGER PRIMARY KEY AUTOINCREMENT,
    \`comic_id\` INTEGER NOT NULL,
    \`episode_id\` INTEGER NOT NULL,
    \`title\` TEXT NOT NULL,
    \`mtime\` INTEGER NOT NULL,
    \`volume\` INTEGER NOT NULL,
    \`chapter\` INTEGER NOT NULL,
    \`url\` TEXT NOT NULL
  );`);

  yield rawDB.run(`CREATE TABLE IF NOT EXISTS \`pages\` (
    \`id\` INTEGER PRIMARY KEY AUTOINCREMENT,
    \`episode_id\` INTEGER NOT NULL,
    \`url\` TEXT NOT NULL
  );`);

  return rawDB;
}


function fromPath (path) {
  return sqlite3(path).then((db) => {
    return co(function * () {
      return yield * createTable(db);
    });
  }).then((db) => {
    return new Database(db);
  });
};


function getInstance () {
  if (gDB) {
    return Promise.resolve(gDB);
  }
  return fromPath('/tmp/atecomic.sqlite').then((db) => {
    gDB = db;
    return gDB;
  });
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
