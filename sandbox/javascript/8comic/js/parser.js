import * as net from './net.js';


export function getLatest (offset) {
  offset = (typeof offset === 'undefined') ? 1 : offset;
  var url = `http://www.comicvip.com/comic/u-${offset}.html`;

  return net.getPage(url).then(function (html) {
    var tmp = html.querySelector('body > table:nth-child(5) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(3) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(2) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1)');
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


export function getSummary (url) {
  return net.getPage(url).then(function (html) {
    var cover = html.querySelector('body > table:nth-child(7) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > table:nth-child(1) > tbody:nth-child(1) > tr:nth-child(1) > td:nth-child(1) > img:nth-child(1)');
    var episodeLinks = html.querySelectorAll('a.Ch');
    // TODO volume, may need two tabs
    // html.querySelectorAll('a.Vol')
    episodeLinks = Array.map(episodeLinks, function (anchor) {
      var action = anchor.getAttribute('onClick');
      action = action.match(/cview\('([^']+)',(\d+)\)/);
      return {
        name: anchor.textContent,
        url: action ? action[1] : '',
        catid: action ? action[2] : '',
      };
    });
    return {
      coverURL: cover ? cover.src : null,
      episodeList: episodeLinks,
    };
  });
}


export function getPagesInEpisode (url, catid) {
  var episodeURL = 'http://www.comicvip.com' + cview(url, catid);
  var ch = episodeURL.match(/ch=(\d+)/);
  if (!ch) {
    return Promise.resolve(null);
  }
  ch = ch[1];

  return net.getPage(episodeURL).then(function (html) {
    var scripts = html.querySelectorAll('script');
    var script = Array.prototype.find.call(scripts, function (script) {
      return script.textContent.indexOf('sp%28%29') >= 0;
    });
    if (!script) {
      return Promise.resolve(null);
    }

    script = script.textContent;
    var chs = script.match(/var chs=(\d+);/);
    var ti = script.match(/var ti=(\d+);/);
    var cs = script.match(/var cs='([^\']+)';/);
    if (!chs || !ti || !cs) {
      return Promise.resolve(null);
    }

    chs = chs[1];
    ti = ti[1];
    cs = cs[1];
    var urls = sp(cs, chs, ch, ti);
    return Promise.resolve(urls);
  });
}


// copied from 8comic
function cview (url, catid) {
  var baseurl = "";
  if (catid == 4 || catid == 6 || catid == 12 || catid == 22) baseurl="/show/cool-";
  if (catid == 1 || catid == 17 || catid == 19 || catid == 21) baseurl="/show/cool-";
  if (catid == 2 || catid == 5 || catid == 7 || catid == 9)  baseurl="/show/cool-";
  if (catid == 10 || catid == 11 || catid == 13 || catid == 14) baseurl="/show/best-manga-";
  if (catid == 3 || catid == 8 || catid == 15 || catid == 16 || catid == 18 || catid == 20) baseurl="/show/best-manga-";
  url = url.replace(".html", "").replace("-", ".html?ch=");
  return baseurl + url;
}

function reurl (url, keyname, keyvalue) {
  var u = url;
  if (u.indexOf('#') == u.length - 1) {
    u = u.substring(0, u.length - 1);
  }
  if (u.indexOf('?') > 0) {
    return u.substring(0, u.indexOf('?')) + '?' + keyname + '=' + keyvalue + ('&' + u.substring(u.indexOf('?') + 1)).replace(eval('/&'+keyname+'=[^&]*/gi'), '');
  } else {
    return u + '?' + keyname + '=' + keyvalue;
  }
}

function j (url, n) {
  return reurl(url, "ch", ch + "-" + n);
}

// cs: chapter information
// chs: total page count
// ch: current episode
// ti: unknown
function sp (cs, chs, ch, ti) {
    var f = 50;
    var c = '';
    var ci = 0;
    var pi = '';
    var ni = '';
    var ps = ''; // total page count

    var cc = cs.length;
    for (let i = 0; i < cc / f; i++) {
        if (ss(cs, i * f, 4) == ch) {
            c = ss(cs, i * f, f, f);
            ci = i;
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
