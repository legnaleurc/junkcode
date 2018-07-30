// ==UserScript==
// @name     Script Blocker
// @version  1
// @grant    none
// @run-at   document-start
// @match    https://2nyan.org/*
// @match    https://avgle.com/*
// ==/UserScript==

'use strict';

const PATTERNS = [
  // setInterval(function(){_0x5b2a75();},0xfa0);
  /setInterval\(function\(\)\{_0x.+\(\);\},0xfa0\);/,
];

const URL_PATTERNS = [
  /avgle-main-ah/,
];

document.addEventListener('beforescriptexecute', event => {
  const url = event.target.getAttribute('src');
  if (shouldBlockURL(url)) {
    event.preventDefault();
    console.info('blocked script');
  }
  const code = event.target.textContent;
  if (shouldBlock(code)) {
    event.preventDefault();
    console.info('blocked script');
  }
});


function shouldBlock (code) {
  if (!code) {
    return false;
  }
  return PATTERNS.some((p) => {
    return !!code.match(p);
  });
}


function shouldBlockURL (url) {
  if (!url) {
    return false;
  }
  return URL_PATTERNS.some((p) => {
    return !!url.match(p);
  });
}
