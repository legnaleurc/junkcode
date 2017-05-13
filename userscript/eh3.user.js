// ==UserScript==
// @name        eh3
// @namespace   sandbox
// @include     https://exhentai.org/g/*
// @include     https://e-hentai.org/g/*
// @version     1
// @grant       GM_xmlhttpRequest
// @grant       GM_addStyle
// ==/UserScript==

'use strict';


searchCache();


async function searchCache () {
  let title = document.querySelector('#gj');
  if (!title) {
    return;
  }

  makeHintArea();

  title = searchKeyword(title.textContent);
  if (!title) {
    markError();
    addHint('no match');
    return;
  }

  markLoading();
  addHint(title);

  try {
    let data = await get('http://acddl.loc.al/api/v1/nodes', {
      pattern: t,
    });
    data = JSON.parse(data);
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
  const blackList = [
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
  const pattern = /\[([^\]]+)\]/g;
  const rv = [];
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
  const pattern = /[a-zA-Z]+-\d+/g;
  const rv = [];
  let tmp = null;
  while ((tmp = pattern.exec(title)) !== null) {
    rv.push(tmp[0]);
  }
  return rv;
}


function makeHintArea () {
  const tagBlock = document.querySelector('#taglist');
  const displayArea = document.createElement('p');
  displayArea.id = 'fake-hint';
  tagBlock.appendChild(displayArea);

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
  const p = document.querySelector('#fake-hint');
  p.classList.remove('loading', 'error');
}


function markLoading () {
  markNormal();

  const p = document.querySelector('#fake-hint');
  p.classList.add('loading');
}


function markError () {
  markNormal();

  const p = document.querySelector('#fake-hint');
  p.classList.add('error');
}


function addHint (message) {
  const p = document.querySelector('#fake-hint');
  const c = document.createElement('pre');
  c.textContent = message;
  p.appendChild(c);
}


function get (url, args) {
  const query = new URLSearchParams();
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
