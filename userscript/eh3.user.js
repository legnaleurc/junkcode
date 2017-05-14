// ==UserScript==
// @name        eh3
// @namespace   sandbox
// @include     https://exhentai.org/g/*
// @include     https://e-hentai.org/g/*
// @version     2
// @grant       GM_xmlhttpRequest
// @grant       GM_addStyle
// ==/UserScript==

'use strict';


Promise.all([
  searchCache(),
  inlineDownload(),
]).catch((e) => {
  console.error(e);
});


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
      pattern: title,
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


async function inlineDownload () {
  await Promise.all([
    inlineArchiveDownload(),
    inlineTorrentDownload(),
  ]);
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


async function inlineArchiveDownload () {
  // get url
  let element = document.querySelector('.g2.gsp > a');
  let url = element.getAttribute('onclick');
  url = url.match(/'([^']+)'/);
  url = url[1];

  // get page information
  let html = await get(url);
  let parser = new DOMParser();
  html = parser.parseFromString(html, 'text/html');
  let form = html.querySelector('#hathdl_form');
  let block = form.nextElementSibling.firstElementChild.firstElementChild.lastElementChild;
  let size = block.children[1].textContent;
  let price = block.children[2].textContent;

  // make UI
  url = form.action;
  block = document.createElement('div');
  block.style.marginLeft = '14px';
  block.style.marginTop = '7px';
  block.style.cursor = 'pointer';
  block.textContent = `${size} / ${price}`;
  block.id = 'btn-dl-archive';
  block.addEventListener('click', (event) => {
    downloadArchive(url).catch((e) => {
      console.warn(e);
    });
  });
  element.insertAdjacentElement('afterend', block);
}


async function inlineTorrentDownload () {
  ;
}


async function downloadArchive (url) {
  let html = await post(url, {
    hathdl_xres: 'org',
  }, {
    Referer: url,
  });
  let msg = 'Downloads should start processing within a couple of minutes.';
  let btn = document.querySelector('#btn-dl-archive');
  if (html.indexOf(msg) >= 0) {
    btn.style.backgroundColor = 'green';
  } else {
    btn.style.backgroundColor = 'green';
  }
  await wait(5000);
  btn.style.backgroundColor = '';
}


function get (url, args) {
  url = new URL(url);
  if (args) {
    Object.keys(args).forEach((k) => {
      url.searchParams.set(k, args[k]);
    });
  }
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
      url: url.toString(),
    });
  });
}


function post (url, args, header) {
  const data = new URLSearchParams();
  if (args) {
    Object.keys(args).forEach((k) => {
      data.set(k, args[k]);
    });
  }
  let headers = {
    'Content-Type': 'application/x-www-form-urlencoded',
  };
  if (header) {
    headers = Object.assign(headers, header);
  }

  return new Promise((resolve, reject) => {
    GM_xmlhttpRequest({
      headers: headers,
      method: 'POST',
      data: data.toString(),
      onload: (response) => {
        resolve(response.responseText);
      },
      onerror: (error) => {
        reject(error);
      },
      url: url.toString(),
    });
  });
}


function wait (msDelay) {
  return new Promise((resolve) => {
    setTimeout(resolve, msDelay);
  });
}
