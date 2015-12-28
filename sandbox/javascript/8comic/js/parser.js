define('parser', ['net'], function (net) {
  'use strict';
  
  function getLatest (offset) {
    offset = (typeof offset === 'undefined') ? 1 : offset;
    var url = `http://www.comicvip.com/comic/u-${offset}.html`;
    
    return net.getPage(url).then(function (html) {
      var tmp = html.querySelector('body > table:nth-child(5) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(3) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(2)');
      tmp = tmp.querySelectorAll('a');
      tmp = Array.map(tmp, function (a) {
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
      return tmp;
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
  
  return {
    getLatest: getLatest,
    getSummary: getSummary,
  };
});
