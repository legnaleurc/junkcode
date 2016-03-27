'use strict';


var request = require('request');
var iconv = require('iconv-lite');
var jsdom = require('jsdom');


function ajax (method, url) {
  if (method === 'GET') {
    return new Promise((resolve, reject) => {
      request.get({
          url: url,
          encoding: null,
        }, function (e, r, body) {
        var newBody = iconv.decode(new Buffer(body), "big5");
        resolve(newBody);
      });
    });
  } else {
    return null;
  }
}


function getPage (url) {
  return ajax('GET', url).then((body) => {
    var document = jsdom.jsdom(body);
    return document;
  });
}


module.exports = {
  getPage: getPage,
};
