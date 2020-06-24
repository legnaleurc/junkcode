// ==UserScript==
// @name        youtube.close
// @namespace   sandbox
// @include     https://www.youtube.com/*
// @version     1
// @grant       unsafeWindow
// @run-at      document-start
// ==/UserScript==

'use strict';


function normalizeURL (url) {
  const currentURL = new URL(window.location.href);
  const targetURL = new URL(url, currentURL);
  const q = targetURL.searchParams.get('q');
  return q;
}


unsafeWindow.open = exportFunction(function (url) {
  const newURL = normalizeURL(url);
  console.info('youtube.open', url, newURL);
  window.location.href = newURL;
  const p = new Proxy({}, {
    get (target, name) {
      console.info('get', target, name);
      return undefined;
    },
  });
  return p;
}, unsafeWindow);
