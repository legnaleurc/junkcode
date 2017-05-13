// ==UserScript==
// @name        nyaa
// @namespace   sandbox
// @include     http://*.nyaa.se/*
// @include     https://*.nyaa.se/*
// @include     http://*.nyaa.eu/*
// @include     https://*.nyaa.eu/*
// @version     1
// @grant       GM_addStyle
// @grant       GM_xmlhttpRequest
// @run-at      document-start
// ==/UserScript==

'use strict';


document.addEventListener('DOMContentLoaded', (event) => {
  tuneLayout();
  searchCache();
});

document.addEventListener('beforescriptexecute', (event) => {
  disableInjection(event);
});


function tuneLayout () {
  document.querySelectorAll('iframe').forEach((n) => {
    n.parentNode.removeChild(n);
  });

  GM_addStyle(`
    #main {
      width: 99%;
    }

    #main > :not(.content) {
      display: none;
    }

    #tabnav {
      float: unset;
    }

    #tabnav > li {
      display: none;
    }

    #tabnav > li:nth-child(1), #tabnav > li:nth-child(8) {
      display: inline;
    }

    .content {
      font-size: 1.2em;
      width: 100%;
    }

    .tlist {
      table-layout: auto;
    }

    .tlistname {
      padding-bottom: 9px;
      padding-top: 9px;
    }

    .tlisticon {
      width: 7.5%;
    }

    .tlisticon img, .tlistdownload img {
      width: 100%;
    }
  `);
}


function disableInjection (event) {
  var url = event.target.src;
  var a = document.createElement('a');
  a.href = url;
  url = a.host;
  if (!url.match(/\.nyaa\.se$/)) {
    event.preventDefault();
    return false;
  }
}


function searchCache () {
  var title = document.querySelector('.viewtorrentname');
  if (!title) {
    return;
  }

  makeHintArea();

  var t = searchKeyword(title.textContent);
  if (!t) {
    markError();
    addHint('no match');
    return;
  }

  markLoading();
  addHint(t);
  get('http://acddl.loc.al/api/v1/nodes', {
    pattern: t,
  }).then((raw) => {
    var data = JSON.parse(raw);
    data.forEach((v) => {
      addHint(v.name);
    });

    markNormal();
  }, (e) => {
    console.error(e);
    markError();
  }).catch((e) => {
    console.error(e);
    markError();
  });
}


function searchKeyword (title) {
  var rv = searchArt(title);
  if (rv.length > 0) {
    return rv[0];
  }

  rv = searchReal(title);
  if (rv.length > 0) {
    return rv[0];
  }

  return null;
}


function searchArt (title) {
  var blackList = [
    /^\d+$/,
    /^(FHD|MP4)\//,
    /^FHD|HD|ADV|CG|RPG|HCG|MMD$/,
    /[23]D-Hentai/,
    /^Crack$/i,
    /^無修正$/,
    /^DL版$/,
    /字幕/,
    / hentai$/i,
    /^BD 1080P$/,
    /$\d{4}-\d{2}-\d{2}$/,
  ];
  var pattern = /\[([^\]]+)\]/g;
  var rv = [];
  var tmp = null;
  while ((tmp = pattern.exec(title)) !== null) {
    tmp = tmp[1];
    let isBlack = blackList.some((p) => {
      return p.test(tmp);
    });
    if (isBlack) {
      continue;
    }
    rv.push(tmp);
  }
  return rv;
}


function searchReal (title) {
  var pattern = /[a-zA-Z]+-\d+/g;
  var rv = [];
  var tmp = null;
  while ((tmp = pattern.exec(title)) !== null) {
    rv.push(tmp[0]);
  }
  return rv;
}


function makeHintArea () {
  var a = document.querySelector('.viewdownloadbutton');
  var b = a.nextElementSibling;
  var c = b.nextElementSibling;
  c.parentNode.removeChild(c);
  b.parentNode.removeChild(b);
  b = a.nextElementSibling;
  a = a.parentNode;

  c = document.createElement('p');
  c.id = 'fake-hint';
  a.insertBefore(c, b);

  GM_addStyle(`
    #fake-hint > pre {
      margin-top: 0.5em;
      margin-bottom: 0.5em;
    }

    #fake-hint.loading {
      background-color: grey;
    }

    #fake-hint.error {
      background-color: red;
      color: black;
    }
  `);
}


function markNormal () {
  var p = document.querySelector('#fake-hint');
  p.classList.remove('loading', 'error');
}


function markLoading () {
  markNormal();

  var p = document.querySelector('#fake-hint');
  p.classList.add('loading');
}


function markError () {
  markNormal();

  var p = document.querySelector('#fake-hint');
  p.classList.add('error');
}


function addHint (message) {
  var p = document.querySelector('#fake-hint');
  var c = document.createElement('pre');
  c.textContent = message;
  p.appendChild(c);
}


function get (url, args) {
  var query = new URLSearchParams();
  Object.keys(args).forEach((k) => {
    query.set(k, args[k]);
  });
  return new Promise((resolve, reject) => {
    GM_xmlhttpRequest({
      header: {
        'Cache-Control': 'no-store',
      },
      method: 'GET',
      onload: (response) => {
        resolve(response.responseText);
      },
      onerror: (error) => {
        reject(error);
      },
      url: url + '?' + query.toString(),
    });
  });
}
