(function () {
  'use strict';
  
  function ajax (method, responseType, url) {
    return new Promise(function (resolve, reject) {
      var xhr = new XMLHttpRequest({
        mozAnon: true,
        mozSystem: true,
      });
      xhr.open(method, url);
      xhr.responseType = responseType;
      xhr.addEventListener('load', function (event) {
        var self = event.target;
        resolve({
          code: self.status,
          body: self.response,
        });
      });
      xhr.addEventListener('error', function (event) {
        reject();
      });
      xhr.send();
    });
  }
  
  function getPage (url) {
    return ajax('GET', 'document', url).then(function (response) {
      return response.body;
    });
  }
  
  function decode (textCodec, arrayBuffer) {
    var view = new DataView(arrayBuffer);
    var decoder = new TextDecoder(textCodec);
    return decoder.decode(view);
  }
  
  function getLatest (offset) {
    offset = (typeof offset === 'undefined') ? 1 : offset;
    var url = `http://www.comicvip.com/comic/u-${offset}.html`;
    
    return getPage(url).then(function (html) {
      var tmp = html.querySelector('body > table:nth-child(5) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(3) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(2)');
      tmp = tmp.querySelectorAll('a');
      tmp = _.map(tmp, function (a) {
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
    return getPage(url).then(function (html) {
      var cover = html.querySelector('body > table:nth-child(7) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > img:nth-child(1)');
      return {
        coverURL: cover ? cover.src : null,
      };
    });
  }
  
  // UI code
  
  function createEntry (entry) {
    var content = document.querySelector('#content');
    var div = document.createElement('div');
    var img = document.createElement('img');
    img.src = entry.data.coverURL;
    div.appendChild(img);
    content.appendChild(div);
  }

  document.addEventListener('DOMContentLoaded', function() {
    getLatest().then(function (__) {
      __.map(createEntry);
    });
  });
  
})();
