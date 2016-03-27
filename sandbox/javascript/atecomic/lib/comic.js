'use strict';


var net = require('./net.js');


function * getLatest (offset) {
  // FIXME what the hell of the first argument?
  // offset = (typeof offset === 'undefined') ? 1 : offset;
  var offset = 1;
  var url = `http://www.comicvip.com/comic/u-${offset}.html`;
  var html = yield net.getPage(url);
  var tmp = html.querySelector('body > table:nth-child(5) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(3) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(2) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1)');
  tmp = tmp.querySelectorAll('a');
  var comicList = Array.prototype.map.call(tmp, (a) => {
    var tmp = a.textContent.match(/\s*(.+)\s*\[\s*(.+)\s*\]\s*/);
    if (tmp) {
      var title = tmp[1].trim();
      var latestEpisode = tmp[2].replace('漫畫', '').trim();
    } else {
      var title = a.textContent;
      var latestEpisode = '';
    }
    return {
      title: title,
      latestEpisode: latestEpisode,
      url: a.href,
    };
  });
  this.body = JSON.stringify(comicList);
}


module.exports = {
  getLatest: getLatest,
};
