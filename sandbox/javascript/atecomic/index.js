'use strict';

var koa = require('koa');
var koaRoute = require('koa-route');

var comic = require('./lib/comic.js');


function main (args) {
  var app = koa();

  app.use(koaRoute.get('/updates', comic.getLatest));

  app.listen(8000);

  return 0;
}

if (!module.parent) {
  return main();
}
