// ==UserScript==
// @name        GonePlurk
// @namespace   sandbox
// @include     http://www.plurk.com/*
// @include     https://www.plurk.com/*
// @version     1
// @grant       unsafeWindow
// @grant       GM_openInTab
// ==/UserScript==

unsafeWindow.Media._hideLink = exportFunction((url) => {
  GM_openInTab(url);
}, unsafeWindow);
unsafeWindow._gaq_push = exportFunction(() => {}, unsafeWindow);
var d = document.querySelector('#timeline_cnt');
var o = new MutationObserver((mutations) => {
  mutations.forEach((mutation) => {
    Array.forEach(mutation.addedNodes, (node) => {
      if (node.classList.contains('ynative')) {
        node.parentNode.removeChild(node);
      }
    });
  });
});
o.observe(d, {
  childList: true,
  subtree: true,
});
