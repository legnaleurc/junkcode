'use strict';

var url = require('url');

var net = require('./net.js');
var db = require('./db.js');
var asyncio = require('./asyncio.js');


function * getLatest (offset) {
  // FIXME what the hell of the first argument?
  // offset = (typeof offset === 'undefined') ? 1 : offset;
  var offset = 1;
  var rootURL = `http://www.comicbus.com/comic/u-${offset}.html`;
  var html = yield net.getPage(rootURL);
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
    var pageURL = url.resolve(rootURL, a.href);
    var id = pageURL.match(/\/(\d+)\.html$/);
    id = id ? id[1] : '0';
    return {
      id: id,
      title: title,
      latestEpisode: latestEpisode,
      url: pageURL,
    };
  });

  comicList = comicList.map((comic) => {
    return new Promise((resolve, reject) => {
      net.getPage(comic.url).then((html) => {
        var summary = getSummary(html);
        comic.coverURL = url.resolve(comic.url, summary.coverURL);
        comic.episodeList = summary.episodeList;
        resolve(comic);
      });
    });
  });
  comicList = yield Promise.all(comicList);

  this.body = JSON.stringify(comicList);
}


function getSummary (html) {
  var cover = html.querySelector('body > table:nth-child(7) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > img:nth-child(1)');
  var episodeLinks = html.querySelectorAll('a.Ch');
  // TODO volume, may need two tabs
  // html.querySelectorAll('a.Vol')
  episodeLinks = Array.prototype.map.call(episodeLinks, function (anchor) {
    var name = anchor.textContent;
    var updateAge = -1;
    var font = anchor.querySelector('font');
    if (font) {
      name = font.textContent;
      let script = anchor.querySelector('script');
      script = script.textContent;
      script = script.match(/isnew\('([^']*)','([^']*)',(\d+)\)/);
      if (script) {
        let dd = script[1];
        let nn = script[2];
        let showimg = parseInt(script[3], 10);
        updateAge = isnew(dd, nn, showimg);
      }
    }
    var action = anchor.getAttribute('onClick');
    action = action.match(/cview\('([^']+)',(\d+)\)/);
    return {
      name: name,
      updateAge: updateAge,
      url: action ? action[1] : '',
      catid: action ? action[2] : '',
    };
  });
  return {
    coverURL: cover ? cover.src : null,
    episodeList: episodeLinks,
  };
}


function getComicIDFromURL (pageURL) {
  var id = pageURL.match(/\/(\d+)\.html$/);
  id = id ? id[1] : '0';
  return parseInt(id, 10);
}


// copied from 8comic
function isnew(dd, nn) {
  var nd = new Date();
  if (nn == null || nn == '') {
    nn = nd.getYear() + '-' + (nd.getMonth() + 1) + '-' + nd.getDate();
  }
  var ddd = dd.split('-');
  var nnn = nn.split('-');
  var y1 = parseInt(ddd[0], 10);
  var m1 = parseInt(ddd[1], 10);
  var d1 = parseInt(ddd[2], 10);
  var y2 = parseInt(nnn[0], 10);
  var m2 = parseInt(nnn[1], 10);
  var d2 = parseInt(nnn[2], 10);
  var dt1 = (y1 - 1900) * 365 + (m1 - 1) * 30 + d1;
  var dt2 = (y2 - 1900) * 365 + (m2 - 1) * 30 + d2;

  return dt2 - dt1;
}


function * fetchAll () {
  var pageURL = 'http://www.comicbus.com/comic/all.html';
  var all = yield net.getPage(pageURL);
  all = all.querySelectorAll('table[id] > tbody > tr > td > a');
  all = Array.prototype.map.call(all, (a) => {
    return {
      id: getComicIDFromURL(a.href),
      url: url.resolve(pageURL, a.href),
    };
  });

  var db_ = db.getInstance();
  yield db_.addRefreshTasks(all);
}


function * pollAll () {
  var db_ = db.getInstance();

  var tasks = yield db_.getDirtyRefreshTasks();
  yield asyncio.asyncForEach(tasks, function * (comic) {
    yield * fetchComic(comic);
  });
}


function * fetchComic (comic) {
  var html = yield net.getPage(comic.url);
  var parsed = {
    id: comic.id,
    url: comic.url,
    title: parseTitle(html),
    coverURL: parseCoverURL(html, comic.url),
    author: parseAuthor(html),
    mtime: parseMTime(html),
    brief: parseBrief(html),
  };
  var episodeList = parseEpisodeList(html, comic.url);
  parsed.chapters = episodeList.chapters;
  parsed.volumes = episodeList.volumes;
}


function parseTitle (comicMainPage) {
  var node = comicMainPage.querySelector('body > table:nth-child(7) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(2) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(2) > font:nth-child(1)');
  return node.textContent.trim();
}


function parseCoverURL (comicMainPage, comicURL) {
  var node = comicMainPage.querySelector('body > table:nth-child(7) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > img:nth-child(1)');
  return url.resolve(comicURL, node.src);
}


function parseAuthor (comicMainPage) {
  var node = comicMainPage.querySelector('body > table:nth-child(7) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(2) > table:nth-child(2) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(3) > td:nth-child(2)');
  return node.textContent.trim();
}


function parseMTime (comicMainPage) {
  var node = comicMainPage.querySelector('body > table:nth-child(7) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(2) > table:nth-child(2) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(11) > td:nth-child(2)');
  var m = node.textContent.match(/^(\d\d\d\d)-(\d\d)-(\d\d)$/);
  m = m.slice(1).map((s) => {
    return parseInt(s, 10);
  });
  var date = new Date(m[0], m[1], m[2]);
  return date;
}


function parseBrief (comicMainPage) {
  var node = comicMainPage.querySelector('body > table:nth-child(7) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(3) > td:nth-child(1)');
  return node.textContent.trim();
}


function parseEpisodeList (comicMainPage, comicURL) {
  var chapterLinks = parseEpisodeListByCategory(comicMainPage, comicURL, 'a.Ch');
  var volumeLinks = parseEpisodeListByCategory(comicMainPage, comicURL, 'a.Vol');

  return {
    chapters: chapterLinks,
    volumes: volumeLinks,
  };
}


function parseEpisodeListByCategory (comicMainPage, comicURL, selector) {
  var links = comicMainPage.querySelectorAll(selector);

  links = Array.prototype.map.call(links, function (anchor) {
    var name = anchor.textContent;
    var updateAge = -1;
    var font = anchor.querySelector('font');
    if (font) {
      name = font.textContent;
      let script = anchor.querySelector('script');
      script = script.textContent;
      script = script.match(/isnew\('([^']*)','([^']*)',(\d+)\)/);
      if (script) {
        let dd = script[1];
        let nn = script[2];
        let showimg = parseInt(script[3], 10);
        updateAge = isnew(dd, nn, showimg);
      }
    }
    var action = anchor.getAttribute('onClick');
    action = action.match(/cview\('([^']+)',(\d+)\)/);
    var pageURL = action ? url.resolve(comicURL, action[1]) : '';
    return {
      name: name.trim(),
      updateAge: updateAge,
      url: pageURL,
      catid: action ? action[2] : '',
    };
  });

  return links;
}


function main () {
  var co = require('co');
  co(function * () {
    // yield * fetchAll();
    yield * pollAll();
  }).then(() => {
  }).catch((e) => {
    throw e;
  });
}


if (!module.parent) {
  return main();
}


module.exports = {
  getLatest: getLatest,
};
