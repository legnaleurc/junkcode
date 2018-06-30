// ==UserScript==
// @name        nyaa
// @namespace   sandbox
// @include     https://*.nyaa.si/*
// @version     1
// @grant       GM.xmlHttpRequest
// @run-at      document-start
// ==/UserScript==

'use strict';


document.addEventListener('DOMContentLoaded', (event) => {
  removeBanner();
  markGarbage();
  searchCache();
});


function removeBanner () {
  let el = document.querySelector('.servers-cost-money1');
  el.previousElementSibling.remove();
  el.remove();
}


async function markGarbage () {
  const filters = await getFilters();

  let fileIcons = document.querySelectorAll('.torrent-file-list i.fa');
  for (let fileIcon of fileIcons) {
    let label = fileIcon.nextSibling.textContent;
    if (!shouldHide(label, filters)) {
      continue;
    }
    let isFile = fileIcon.classList.contains('fa-file');
    let li = null;
    if (isFile) {
      li = fileIcon.parentElement;
    } else {
      li = fileIcon.parentElement.parentElement;
    }
    li.style.opacity = 0.5;
  }
}


async function getFilters () {
  let rv = await get('http://dfd.loc.al/api/v1/filters', {});
  rv = JSON.parse(rv);
  rv = Object.keys(rv).map((key) => {
    return new RegExp(rv[key], 'i');
  });
  return rv;
}



function shouldHide (name, filters) {
  return filters.some((re) => {
    return name.trim().match(re);
  });
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

  addStyle(`
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
  query = query.toString();
  if (query) {
    query = '?' + query;
  }
  return new Promise((resolve, reject) => {
    GM.xmlHttpRequest({
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
      url: url + query,
    });
  });
}


function addStyle (text) {
  let style = document.createElement('style');
  style.type = 'text/css';
  style.textContent = text;
  document.head.appendChild(style);
}
