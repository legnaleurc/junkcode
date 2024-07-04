// ==UserScript==
// @name        eh query
// @namespace   sandbox
// @include     https://exhentai.org/g/*
// @include     https://e-hentai.org/g/*
// @version     2
// @grant       GM.xmlHttpRequest
// ==/UserScript==

'use strict';


const NODES_URL = 'http://dvd.localhost/api/v1/nodes';


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
  if (!title.textContent) {
    title = document.querySelector('#gn');
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
    let data = await get(NODES_URL, {
      name: title,
      fuzzy: true,
    });
    data = JSON.parse(data);
    data.forEach((v) => {
      addTextToSearchHint(`${v.parent_path}/${v.name}`);
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


/**
 * search keyword
 * @param {string} title title
 * @returns {string | null} keyword
 */
function searchKeyword (title) {
  let rv = searchArtKeyword(title);
  if (rv.length > 0) {
    return rv[0];
  }

  rv = searchRealKeyword(title);
  if (rv.length > 0) {
    return rv[0];
  }

  return null;
}


/**
 * search art keyword
 * @param {string} title title
 * @returns {string[]} keyword
 */
function searchArtKeyword (title) {
  const blackList = [
    /^\d+$/,
    /^(FHD|MP4)\//,
    /[23]D-Hentai/,
    /^Crack$/i,
    /^無修正$/,
    /^DL版$/,
    /字幕/,
    / hentai$/i,
    /^BD 1080P$/,
    /^\d{2}-\d{2}-\d{2}$/,
    /^\d{4}-\d{2}-\d{2}$/,
  ];
  const pattern = /\[([^\]]+)\]/g;
  const rv = [];
  let tmp = null;
  while ((tmp = pattern.exec(title)) !== null) {
    tmp = tmp[1];
    const isBlack = blackList.some((p) => {
      return p.test(tmp);
    });
    if (isBlack) {
      continue;
    }
    rv.push(tmp);
  }
  return rv;
}


/**
 * search real keyword
 * @param {string} title title
 * @returns {string[]} keyword
 */
function searchRealKeyword (title) {
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
  box.addEventListener('transitionend', (event) => {
    let e = event.target;
    e.classList.remove('success', 'error');
  });
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

  addStyle(`
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

    #blk-archive {
      transition: background-color 2s;
    }

    #blk-archive.success {
      background-color: green;
    }

    #blk-archive.error {
      background-color: red;
    }

    #btn-dl-archive {
      cursor: pointer;
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


/**
 * add text to search hint
 * @param {string} message message
 */
function addTextToSearchHint (message) {
  const p = document.querySelector('#blk-search');
  const c = document.createElement('pre');
  c.textContent = message;
  p.appendChild(c);
}


async function inlineArchiveDownload () {
  // get URL
  const element = document.querySelector('.g2.gsp > a');
  let url = element.getAttribute('onclick');
  url = url.match(/'([^']+)'/);
  url = url[1];

  // get page information
  let html = await get(url);
  const parser = new DOMParser();
  html = parser.parseFromString(html, 'text/html');
  const form = html.querySelector('#hathdl_form');
  const block = form.nextElementSibling.firstElementChild.firstElementChild.lastElementChild;
  const size = block.children[1].textContent;
  const price = block.children[2].textContent;

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
  const html = await get(url);
  const parser = new DOMParser();
  const rv = parser.parseFromString(html, 'text/html');

  // make UI
  makeDownloadTorrentLink(rv);
}


/**
 * make download archive link
 * @param {string} url url
 * @param {string} size size
 * @param {string} price price
 */
function makeDownloadArchiveLink (url, size, price) {
  const block = document.createElement('div');
  block.textContent = `${size} / ${price}`;
  block.id = 'btn-dl-archive';
  block.addEventListener('click', (event) => {
    downloadArchive(url).catch((e) => {
      console.warn(e);
    });
  });
  const element = document.querySelector('#blk-archive');
  element.appendChild(block);
}


/**
 * make download torrent link
 * @param {Document} html html
 */
function makeDownloadTorrentLink (html) {
  const blackUploader = [
    '枯树昏鸦',
  ];
  const blocks = html.querySelectorAll('#torrentinfo > div:nth-child(1) > form');
  const box = document.createDocumentFragment();
  blocks.forEach((block) => {
    let uploader = block.querySelector('div:nth-child(1) > table:nth-child(2) > tbody:nth-child(1) > tr:nth-child(2) > td:nth-child(1)');
    uploader = uploader.firstChild.nextSibling.textContent.trim();
    const skip = blackUploader.some(u => u === uploader);
    if (skip) {
      return;
    }
    block = document.importNode(block, true);
    lowlightUploader(uploader, block);
    box.appendChild(block);
  });
  const element = document.querySelector('#blk-torrent');
  element.appendChild(box);
}


/**
 * lowlight uploader
 * @param {string} uploader uploader
 * @param {HTMLElement} block block
 */
function lowlightUploader(uploader, block) {
  const isLowlight = uploader !== 'milannews';
  if (!isLowlight) {
    return;
  }
  const box = block.querySelector('form > div');
  box.style.borderStyle = 'dashed';
}


/**
 * download archive
 * @param {string} url url
 */
async function downloadArchive (url) {
  const html = await post(url, {
    hathdl_xres: 'org',
  }, {
    Referer: url,
  });
  const msg = 'Downloads should start processing within a couple of minutes.';
  const block = document.querySelector('#blk-archive');
  if (html.indexOf(msg) >= 0) {
    block.classList.add('success');
  } else {
    console.error(html);
    block.classList.add('error');
  }
}


/**
 * get
 * @param {string} url url
 * @param {Record<string, string>} args args
 * @returns {Promise<string>} response
 */
function get (url, args) {
  const url_ = new URL(url);
  if (args) {
    Object.keys(args).forEach((k) => {
      url_.searchParams.set(k, args[k]);
    });
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
      url: url_.toString(),
    });
  });
}


/**
 * post
 * @param {string} url url
 * @param {Record<string, string>} args args
 * @param {Record<string, string> | undefined} header header
 * @returns {Promise<string>} response
 */
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
    GM.xmlHttpRequest({
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


/**
 * wait
 * @param {number} msDelay delay
 * @returns {Promise<void>}
 */
function wait (msDelay) {
  return new Promise((resolve) => {
    setTimeout(resolve, msDelay);
  });
}


/**
 * add style
 * @param {string} text text
 */
function addStyle (text) {
  const style = document.createElement('style');
  style.textContent = text;
  document.head.appendChild(style);
}
