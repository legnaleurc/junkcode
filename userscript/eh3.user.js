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


main().catch((e) => {
  console.error(e);
});


async function main () {
  makeHintArea();
  await Promise.all([
    searchCache(),
    inlineDownload(),
  ]);
}


async function searchCache () {
  let title = document.querySelector('#gj');
  if (!title) {
    return;
  }

  title = searchKeyword(title.textContent);
  if (!title) {
    markSearchError();
    addTextToSearchHint('no match');
    return;
  }

  markSearchLoading();
  addTextToSearchHint(title);

  try {
    let data = await get('http://acddl.loc.al/api/v1/nodes', {
      pattern: title,
    });
    data = JSON.parse(data);
    data.forEach((v) => {
      addTextToSearchHint(v.name);
    });
    markSearchNormal();
  } catch (e) {
    console.error(e);
    markSearchError();
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
  const infoBlock = document.querySelector('.gm');
  const displayArea = document.createDocumentFragment();

  let box = document.createElement('div');
  box.id = 'blk-archive';
  box.classList.add('gm');
  displayArea.appendChild(box);

  box = document.createElement('div');
  box.id = 'blk-torrent';
  box.classList.add('gm');
  displayArea.appendChild(box);

  box = document.createElement('div');
  box.id = 'blk-search';
  box.classList.add('gm');
  displayArea.appendChild(box);

  infoBlock.parentElement.insertBefore(displayArea, infoBlock.nextElementSibling);

  GM_addStyle(`
    #blk-search {
      text-align: left;
    }

    #blk-search > pre {
      margin-top: 0.5em;
      margin-bottom: 0.5em;
    }

    #blk-search.loading {
      background-color: grey;
    }

    #blk-search.error {
      background-color: red;
      color: black;
    }
  `);
}


function markSearchNormal () {
  const p = document.querySelector('#blk-search');
  p.classList.remove('loading', 'error');
}


function markSearchLoading () {
  markSearchNormal();

  const p = document.querySelector('#blk-search');
  p.classList.add('loading');
}


function markSearchError () {
  markSearchNormal();

  const p = document.querySelector('#blk-search');
  p.classList.add('error');
}


function addTextToSearchHint (message) {
  const p = document.querySelector('#blk-search');
  const c = document.createElement('pre');
  c.textContent = message;
  p.appendChild(c);
}


async function inlineArchiveDownload () {
  // get URL
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
  makeDownloadArchiveLink(form.action, size, price);
}


async function inlineTorrentDownload () {
  // get URL
  let element = document.querySelector('#gd5');
  element = element.children[2];
  element = element.querySelector('a');
  let url = element.getAttribute('onclick');
  url = url.match(/'([^']+)'/);
  url = url[1];

  // get page information
  let html = await get(url);
  let parser = new DOMParser();
  html = parser.parseFromString(html, 'text/html');

  // make UI
  makeDownloadTorrentLink(html);
}


function makeDownloadArchiveLink (url, size, price) {
  let block = document.createElement('div');
  block.style.cursor = 'pointer';
  block.textContent = `${size} / ${price}`;
  block.id = 'btn-dl-archive';
  block.addEventListener('click', (event) => {
    downloadArchive(url).catch((e) => {
      console.warn(e);
    });
  });
  let element = document.querySelector('#blk-archive');
  element.appendChild(block);
}


function makeDownloadTorrentLink (html) {
  let blocks = html.querySelectorAll('#torrentinfo > div:nth-child(1) > form');
  let box = document.createDocumentFragment();
  blocks.forEach((block) => {
    block = document.importNode(block, true);
    box.appendChild(block);
  });
  let element = document.querySelector('#blk-torrent');
  element.appendChild(box);
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
