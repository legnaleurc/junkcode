'use strict';

var url = require('url');

var net = require('./net.js');
var db = require('./db.js');
var asyncio = require('./asyncio.js');


function * getUpdates () {
  let comicList = [];
  for (let i = 1; i <= 5; ++i) {
    comicList = comicList.concat(yield * getUpdatesByPage(i));
  }
  comicList = comicList.filter((value, index, self) => {
    return self.findIndex((aValue) => {
      return value.id === aValue.id;
    }) === index;
  });

  var db_ = yield db.getInstance();

  yield asyncio.forEach(comicList, function * (comic) {
    console.info(comic);
    comic = yield * fetchComic(comic);
    console.info(comic);
    // TODO transaction?
    yield db_.updateComic(comic);
    yield db_.clearComic(comic.id);
  });
}


function * getUpdatesByPage (page) {
  var rootURL = `http://www.comicbus.com/comic/u-${page}.html`;
  var html = yield net.getPage(rootURL);
  var tmp = html.querySelector('body > table:nth-child(5) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(3) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(2) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1)');
  tmp = tmp.querySelectorAll('a');

  var comicList = Array.prototype.map.call(tmp, (a) => {
    var pageURL = url.resolve(rootURL, a.href);
    var id = getComicIDFromURL(pageURL);
    return {
      id: id,
      url: pageURL,
    };
  });

  return comicList;
}


function getComicIDFromURL (pageURL) {
  var id = pageURL.match(/\/(\d+)\.html$/);
  id = id ? id[1] : '0';
  return parseInt(id, 10);
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

  var db_ = yield db.getInstance();
  yield db_.addRefreshTasks(all);
}


function * pollAll () {
  var db_ = yield db.getInstance();

  var tasks = yield db_.getDirtyRefreshTasks();
  yield asyncio.forEach(tasks, function * (comic) {
    console.info(comic);
    comic = yield * fetchComic(comic);
    console.info(comic);
    // TODO transaction?
    yield db_.updateComic(comic);
    yield db_.clearComic(comic.id);
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
  parsed.episodes = yield * parseEpisodeList(html, comic.url);
  return parsed;
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


function * parseEpisodeList (comicMainPage, comicURL) {
  var chapterFromPage = new Map();
  var volumeLinks = yield * parseEpisodeListByCategory(comicMainPage, comicURL, 'a.Vol', chapterFromPage);
  volumeLinks = volumeLinks.map((episode) => {
    episode.isVolume = true;
    episode.isChapter = false;
    return episode;
  });
  var chapterLinks = yield * parseEpisodeListByCategory(comicMainPage, comicURL, 'a.Ch', chapterFromPage);
  chapterLinks = chapterLinks.map((episode) => {
    episode.isVolume = false;
    episode.isChapter = true;
    return episode;
  });

  return volumeLinks.concat(chapterLinks);
}


function * parseEpisodeListByCategory (comicMainPage, comicURL, selector, chapterFromPage) {
  var links = comicMainPage.querySelectorAll(selector);
  links = Array.prototype.slice.call(links);

  links = yield asyncio.map(links, function * (anchor) {
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
    var detail = cview(action[1], parseInt(action[2], 10));
    var pageFunction = yield * getPageFunction(chapterFromPage, detail.commonURL);
    var pages = pageFunction(detail.ch);

    return {
      id: parseInt(detail.ch, 10),
      title: name.trim(),
      updateAge: updateAge,
      url: detail.url,
      pages: pages,
    };
  });

  return links;
}


function * getPageFunction (chapterFromPage, pageURL) {
  if (chapterFromPage.has(pageURL)) {
    return chapterFromPage.get(pageURL);
  }

  var html = yield net.getPage(pageURL);
  var scripts = html.querySelectorAll('script');
  var script = Array.prototype.find.call(scripts, function (script) {
    return script.textContent.indexOf('sp%28%29') >= 0;
  });
  if (!script) {
    return null;
  }

  script = script.textContent;
  var chs = script.match(/var chs=(\d+);/);
  var ti = script.match(/var ti=(\d+);/);
  var cs = script.match(/var cs='([^\']+)';/);
  if (!chs || !ti || !cs) {
    return null;
  }

  chs = chs[1];
  ti = ti[1];
  cs = cs[1];

  var fn = (ch) => {
    return sp(cs, chs, ch, ti);
  };
  chapterFromPage.set(pageURL, fn);
  return fn;
}


function main () {
  var co = require('co');
  co(function * () {
    yield * fetchAll();
    yield * pollAll();
    yield * getUpdates();
  }).then(() => {
  }).catch((e) => {
    throw e;
  });
}


// translated from 8comic
function isnew (dd, nn) {
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


function cview (episodePath, catid) {
  var baseURL = 'http://v.comicbus.com';

  switch (catid) {
    case 4:
    case 6:
    case 12:
    case 22:
      baseURL += '/online/Domain-';
      break;
    case 1:
    case 17:
    case 19:
    case 21:
    case 3:
    case 8:
    case 15:
    case 16:
    case 18:
    case 20:
      baseURL += '/online/finance-';
      break;
    case 2:
    case 5:
    case 7:
    case 9:
    case 10:
    case 11:
    case 13:
    case 14:
      baseURL += '/online/insurance-';
      break;
  }

  var part = episodePath.replace('.html', '').split('-');
  var commonURL = baseURL + part[0] + '.html';
  var ch = part[1];

  return {
    url: commonURL + '?ch=' + ch,
    commonURL: commonURL,
    ch: ch,
  };
}


// cs: chapter information
// chs: total page count
// ch: current episode
// ti: unknown
function sp (cs, chs, ch, ti) {
  var f = 50;
  var c = '';
  var ps = ''; // total page count

  var cc = cs.length;
  for (let i = 0; i < cc / f; i++) {
    if (ss(cs, i * f, 4) == ch) {
      c = ss(cs, i * f, f, f);
      break;
    }
  }
  if (c == '') {
    c = ss(cs, cc - f, f);
    ch = chs;
  }
  ps = ss(c, 7, 3);
  ps = parseInt(ps, 10);
  var a = [];
  for (let p = 1; p <= ps; ++p) {
    a.push(si(c, ti, p, f));
  }
  return a;
}


function ss (a, b, c, d) {
  var e = a.substring(b, b + c);
  return d == null ? e.replace(/[a-z]*/gi, "") : e;
}


function si (c, ti, p, f) {
  return 'http://img' + ss(c, 4, 2) + '.6comic.com:99/' + ss(c, 6, 1) + '/' + ti + '/' + ss(c, 0, 4) + '/' + nn(p) + '_' + ss(c, mm(p) + 10, 3, f) + '.jpg';
}


function nn (n) {
  return n < 10 ? '00' + n : n < 100 ? '0' + n : n;
}


function mm (p) {
  return (parseInt((p - 1) / 10) % 10) + (((p - 1) % 10) * 3);
}
// end


if (!module.parent) {
  return main();
}


module.exports = {
  getUpdates: getUpdates,
  fetchAll: fetchAll,
  pollAll: pollAll,
};
