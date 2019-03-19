// ==UserScript==
// @name        eh hide cover
// @namespace   sandbox
// @include     https://exhentai.org/*
// @include     https://e-hentai.org/*
// @version     1
// @grant       none
// ==/UserScript==

'use strict';


const HIDDEN = 'rgb(0, 0, 0)';
const HIGHLIGHT = 'rgb(3, 71, 207)';


main();


function main () {
  const items = document.querySelectorAll('.gl1t');
  items.forEach((item) => {
    if (hasHiddenFlags(item)) {
      item.style.opacity = 0.2;
    } else if (hasHighlightFlags(item)) {
      const tmp = item.querySelector('.gl3t');
      tmp.style.borderColor = 'lime';
      tmp.style.borderWidth = '5px';
    }
  });
}


function hasHiddenFlags (item) {
  const labels = item.querySelectorAll('.gt');
	return Array.prototype.some.call(labels, label => label.style.borderColor === HIDDEN);
}


function hasHighlightFlags (item) {
  const labels = item.querySelectorAll('.gt');
	return Array.prototype.some.call(labels, label => label.style.borderColor === HIGHLIGHT);
}
