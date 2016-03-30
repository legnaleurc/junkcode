'use strict';

var fetch = require('node-fetch');
var jsdom = require('jsdom');


function getPage (url) {
  return fetch(url).then((response) => {
    return response.text();
  }).then((html) => {
    var document = jsdom.jsdom(html);
    return document;
  });
}


module.exports = {
  getPage: getPage,
};
