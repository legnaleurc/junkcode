// ==UserScript==
// @name        eh no ad
// @namespace   sandbox
// @include     http://g.e-hentai.org/s/*
// @include     http://g.exhentai.org/s/*
// @version     1
// @grant       none
// ==/UserScript==

var i = document.querySelectorAll('iframe');
Array.prototype.forEach.call(i, (node) => {
  node.parentNode.removeChild(node);
});
