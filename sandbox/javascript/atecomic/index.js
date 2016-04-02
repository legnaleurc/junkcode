'use strict';

var koa = require('koa');
var koaRoute = require('koa-route');
var co = require('co');

var api = require('./lib/api.js');
var comic = require('./lib/comic.js');


function main (args) {
  co(function * () {
    // yield * comic.fetchAll();
    // yield * comic.pollAll();
    // yield * comic.getUpdates();
  }).then(() => {
    var app = koa();

    app.use(koaRoute.get('/updates', api.getUpdates));
    app.use(koaRoute.get('/comics', api.getComics));
    app.use(koaRoute.get('/comics/:comic_id', api.getComic));
    app.use(koaRoute.get('/comics/:comic_id/episodes', api.getEpisodes));
    app.use(koaRoute.get('/comics/:comic_id/episodes/:episode_id/pages', api.getPages));

    app.listen(1337);
  });

  return 0;
}

if (!module.parent) {
  return main();
}
