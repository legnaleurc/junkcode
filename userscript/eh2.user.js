// ==UserScript==
// @name        eh no ad
// @namespace   sandbox
// @include     http://g.e-hentai.org/s/*
// @include     http://g.exhentai.org/s/*
// @version     1
// @grant       none
// ==/UserScript==

const nodes = document.querySelectorAll('iframe');
nodes.forEach((node) => {
  node.remove();
});
