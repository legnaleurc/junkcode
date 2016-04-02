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

  var db_ = yield db.getInstance();
  var comic = yield db_.getComic(comicID);
  this.body = JSON.stringify(comic) + '\n';
}


function * getEpisodes (comicID, next) {
  if (this.method !== 'GET') {
    return yield next;
  }

  var db_ = yield db.getInstance();
  var episodes = yield db_.getEpisodes(comicID);
  this.body = JSON.stringify(episodes) + '\n';
}


function * getPages (comicID, episodeID, next) {
  if (this.method !== 'GET') {
    return yield next;
  }

  var db_ = yield db.getInstance();
  var pages = yield db_.getPages(episodeID);
  this.body = JSON.stringify(pages) + '\n';
}


module.exports = {
  getUpdates: getUpdates,
  getComic: getComic,
  getEpisodes: getEpisodes,
  getPages: getPages,
};
