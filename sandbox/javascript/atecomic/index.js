'use strict';

var koa = require('koa');
var koaRoute = require('koa-route');

var api = require('./lib/api.js');


function main (args) {
  var app = koa();

  app.use(koaRoute.get('/updates', api.getUpdates));
  app.use(koaRoute.get('/comics/:comic_id', api.getComic));
  app.use(koaRoute.get('/comics/:comic_id/episodes', api.getEpisodes));
  app.use(koaRoute.get('/comics/:comic_id/episodes/:episode_id/pages', api.getPages));

  app.listen(8000);

  return 0;
}

if (!module.parent) {
  return main();
}
