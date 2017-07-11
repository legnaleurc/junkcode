// ==UserScript==
// @name        nyaa
// @namespace   sandbox
// @include     https://*.nyaa.si/*
// @version     1
// @grant       GM_addStyle
// @grant       GM_xmlhttpRequest
// @run-at      document-start
// ==/UserScript==

'use strict';


document.addEventListener('DOMContentLoaded', (event) => {
  // tuneLayout();
  searchCache();
});

document.addEventListener('beforescriptexecute', (event) => {
  // disableInjection(event);
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


async function searchCache () {
  let title = document.querySelector('body > .container > div:nth-child(1) .panel-title');
  if (!title) {
    return;
  }

  makeHintArea();

  let t = searchKeyword(title.textContent);
  if (!t) {
    markError();
    addHint('no match');
    return;
  }

  markLoading();
  addHint(t);

  try {
    let raw = await get('http://ddld.loc.al/api/v1/nodes', {
      pattern: t,
    });
    let data = JSON.parse(raw);
    data.forEach((v) => {
      addHint(v.name);
    });
    markNormal();
  } catch (e) {
    console.error(e);
    markError();
  }
}


function searchKeyword (title) {
  let rv = searchArt(title);
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
  let blackList = [
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
  let pattern = /\[([^\]]+)\]/g;
  let rv = [];
  let tmp = null;
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
  let pattern = /[a-zA-Z]+-\d+/g;
  let rv = [];
  let tmp = null;
  while ((tmp = pattern.exec(title)) !== null) {
    rv.push(tmp[0]);
  }
  return rv;
}


function makeHintArea () {
  let a = document.querySelector('body > .container > div:nth-child(2)');
  let c = document.createElement('div');
  c.id = 'fake-hint';
  c.classList.add('panel', 'panel-default');
  a.parentNode.insertBefore(c, a);

  GM_addStyle(`
    #fake-hint > pre {
      padding: initial;
      border: initial;
      background-color: initial;
      overflow: initial;
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
  let p = document.querySelector('#fake-hint');
  p.classList.remove('loading', 'error');
}


function markLoading () {
  markNormal();

  let p = document.querySelector('#fake-hint');
  p.classList.add('loading');
}


function markError () {
  markNormal();

  let p = document.querySelector('#fake-hint');
  p.classList.add('error');
}


function addHint (message) {
  let p = document.querySelector('#fake-hint');
  let c = document.createElement('pre');
  c.textContent = message;
  p.appendChild(c);
}


function get (url, args) {
  let query = new URLSearchParams();
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
