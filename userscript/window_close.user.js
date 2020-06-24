// ==UserScript==
// @name        window.close
// @namespace   sandbox
// @include     http://*
// @include     https://*
// @exclude     https://www.office.com/*
// @exclude     https://onedrive.live.com/*
// @exclude     https://www.youtube.com/*
// @version     1
// @grant       unsafeWindow
// @run-at      document-start
// ==/UserScript==

'use strict';

function getDialog () {
  let div = document.querySelector('#close-overlay');
  if (div) {
    return div;
  }
  div = document.createElement('div');
  div.id = 'close-overlay';
  div.style.position = 'fixed';
  div.style.top = 0;
  div.style.left = 0;
  div.style.width = '100%';
  div.style.zIndex = 65535;
  div.style.backgroundColor = 'black';
  div.style.color = 'white';
  document.body.appendChild(div);
  return div;
}

function appendDialog (url) {
  const div = getDialog();
  const p = document.createElement('p');
  p.textContent = url;
  div.appendChild(p);
}

unsafeWindow.open = exportFunction(function (url) {
  console.info('window.open', url);
  appendDialog(url);
  const p = new Proxy({}, {
    get (target, name) {
      console.info('get', target, name);
      return undefined;
    },
  });
  return p;
}, unsafeWindow);
