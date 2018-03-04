// ==UserScript==
// @name     Script Blocker
// @version  1
// @grant    none
// @run-at   document-start
// @match    https://2nyan.org/*
// ==/UserScript==

'use strict';

document.addEventListener('beforescriptexecute', event => {
  const code = event.target.textContent;
  if (shouldBlock(code)) {
    event.preventDefault();
    console.info('blocked script');
  }
});


function shouldBlock (code) {
  // setInterval(function(){_0x5b2a75();},0xfa0);
  return !!code.match(/setInterval\(function\(\)\{_0x.+\(\);\},0xfa0\);/);
}
