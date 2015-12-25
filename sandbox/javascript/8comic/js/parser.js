(function (exports) {
  'use strict';
  
  var net = exports.net;
  var parser = {};
  
  function getLatest (offset) {
    offset = (typeof offset === 'undefined') ? 1 : offset;
    var url = `http://www.comicvip.com/comic/u-${offset}.html`;
    
    return net.getPage(url).then(function (html) {
      var tmp = html.querySelector('body > table:nth-child(5) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(3) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(2)');
      tmp = tmp.querySelectorAll('a');
      tmp = Array.map(tmp, function (a) {
        var title = a.textContent.trim();
        var url = a.href;
        return {
          title: title,
          url: url,
        };
      });
      return tmp;
    }).then(function (comicList) {
      return new Promise(function (resolve, reject) {
        Promise.all(comicList.map(function (comic) {
          return getSummary(comic.url);
        })).then(function (comicDetailList) {
          comicList.forEach(function (value, index) {
            value.data = comicDetailList[index];
          });
          resolve(comicList);
        });
      });
    });
  }
  
  function getSummary (url) {
    return net.getPage(url).then(function (html) {
      var cover = html.querySelector('body > table:nth-child(7) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > img:nth-child(1)');
      return {
        coverURL: cover ? cover.src : null,
      };
    });
  }
  
  parser.getLatest = getLatest;
  exports.parser = parser;
  
})(window);
