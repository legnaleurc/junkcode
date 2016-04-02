'use strict';

var db = require('./db.js');


function * getUpdates (next) {
  if (this.method !== 'GET') {
    return yield next;
  }

  var db_ = yield db.getInstance();
  var comics = yield db_.getLatestComics();
  this.body = JSON.stringify(comics) + '\n';
}


function * getComic (comicID, next) {
  if (this.method !== 'GET') {
    return yield next;
  }
  console.info(arguments);
  this.body = 'hello\n';
}


function * getEpisodes (comicID, episodeID, next) {
  if (this.method !== 'GET') {
    return yield next;
  }
  console.info(arguments);
  this.body = 'hello\n';
}


function * getPages (comicID, episodeID, next) {
  if (this.method !== 'GET') {
    return yield next;
  }
  console.info(arguments);
  this.body = 'hello\n';
}


module.exports = {
  getUpdates: getUpdates,
  getComic: getComic,
  getEpisodes: getEpisodes,
  getPages: getPages,
};
