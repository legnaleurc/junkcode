// ==UserScript==
// @name        script.close
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

const blackList = [
  'script',
  'iframe',
];
const createElement = unsafeWindow.Document.prototype.createElement;

unsafeWindow.Document.prototype.createElement = exportFunction(function (name) {
  if (blackList.indexOf(name) < 0) {
    return createElement.call(this, name);
  }
  console.info('document.createElement', name);
  const p = new Proxy({}, {
    get (target, name) {
      console.info('get', target, name);
      return undefined;
    },
  });
  return p;
}, unsafeWindow);
