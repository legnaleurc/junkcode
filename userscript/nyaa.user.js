// ==UserScript==
// @name        nyaa
// @namespace   sandbox
// @include     https://*.nyaa.si/*
// @version     1
// @grant       GM.xmlHttpRequest
// @run-at      document-start
// ==/UserScript==

'use strict';


const URL_ = {
  filters: 'http://dfd.local/api/v1/filters',
  nodes: 'http://dvd.local/api/v1/nodes',
};
const TOKEN = '';
const DFD_USERNAME = '';
const DFD_PASSWORD = '';


document.addEventListener('DOMContentLoaded', (event) => {
  markGarbage();
  searchCache();
});


async function markGarbage () {
  const filters = await getFilters();

  let fileIcons = document.querySelectorAll('.torrent-file-list i.fa');
  for (let fileIcon of fileIcons) {
    let label = fileIcon.nextSibling.textContent;
    if (!shouldHide(label, filters)) {
      continue;
    }
    let li = getIconParent(fileIcon);
    li.style.opacity = 0.5;
  }
}


async function getFilters () {
  const headers = {};
  if (DFD_USERNAME && DFD_PASSWORD) {
    headers['Authorization'] = `Basic ${btoa(`${DFD_USERNAME}:${DFD_PASSWORD}`)}`;
  }
  let rv = await get(URL_.filters, {}, headers);
  rv = JSON.parse(rv);
  rv = rv.map((value) => {
    return new RegExp(value.regexp, 'i');
  });
  return rv;
}


function shouldHide (name, filters) {
  return filters.some((re) => {
    return name.trim().match(re);
  });
}


function getIconParent (icon) {
  let isFile = icon.classList.contains('fa-file');
  if (isFile) {
    return icon.parentElement;
  } else {
    return icon.parentElement.parentElement;
  }
}


async function searchCache () {
  let title = document.querySelector('div.panel:nth-child(6) > div:nth-child(1) > h3:nth-child(1)');
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
    const data = await queryNodesApi(t);
    data.forEach((v) => {
      addHint(`${v.parent_path}/${v.name}`);
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
  let a = document.querySelector('div.panel:nth-child(7)');
  let c = document.createElement('div');
  c.id = 'fake-hint';
  c.classList.add('panel', 'panel-default');
  a.parentNode.insertBefore(c, a);

  addStyle(`
    #fake-hint > p {
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
  let c = document.createElement('p');
  c.textContent = message;
  p.appendChild(c);
}


/**
 * queryNodesApi
 * @param {string} title title
 * @returns {Promise<Array<Object>>} response
 */
async function queryNodesApi (title) {
  const args = {
    name: title,
    fuzzy: true,
  };
  const headers = {};
  if (TOKEN) {
    headers['Authorization'] = `Token ${TOKEN}`;
  }
  const data = await get(URL_.nodes, args, headers);
  return JSON.parse(data);
}


function get (url, args, header) {
  let query = new URLSearchParams();
  Object.keys(args).forEach((k) => {
    query.set(k, args[k]);
  });
  query = query.toString();
  if (query) {
    query = '?' + query;
  }
  let headers = {
    'Cache-Control': 'no-store',
  };
  if (header) {
    headers = Object.assign(headers, header);
  }
  return new Promise((resolve, reject) => {
    GM.xmlHttpRequest({
      headers,
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
