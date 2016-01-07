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

  return net.getPage(episodeURL).then(function (html) {
    var firstPage = html.querySelector('#TheImg');
    var firstPageURL = firstPage.src;
    var pageIndex = html.querySelector('#pageindex');
    // we already have the first page
    var indexes = Array.slice(pageIndex.children, 1);
    indexes = indexes.map(function (option) {
      var ch = option.value;
      var url = j(episodeURL, ch);
      return net.getPage(url).then(function (html) {
        var page = html.querySelector('#TheImg');
        return page.src;
      });
    });
    // add back the placeholder of the first page
    indexes.unshift(Promise.resolve(firstPageURL));
    return Promise.all(indexes);
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
