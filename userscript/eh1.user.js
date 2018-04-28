// ==UserScript==
// @name        eh hide cover
// @namespace   sandbox
// @include     https://exhentai.org/*
// @include     https://e-hentai.org/*
// @version     1
// @grant       none
// ==/UserScript==

'use strict';

main();


function main () {
  const items = document.querySelectorAll('.id1');

  items.forEach((item) => {
    if (hasHiddenFlags(item) || hasHiddenCategories(item)) {
      item.style.opacity = 0.2;
    } else if (hasHighlightFlags(item)) {
      item.style.borderColor = 'lime';
    }
  });
}


function hasHiddenFlags (item) {
  let marks = item.querySelectorAll('.tft');
  marks = Array.prototype.filter.call(marks, (mark) => {
    return mark.style.backgroundPosition === '0px -1px';
  });
  return marks.length > 0;
}


function hasHiddenCategories (item) {
  let marks = item.querySelectorAll('.id41');
  marks = Array.prototype.filter.call(marks, (mark) => {
    return mark.title === 'Western';
  });
  return marks.length > 0;
}


function hasHighlightFlags (item) {
  let marks = item.querySelectorAll('.tft');
  marks = Array.prototype.filter.call(marks, (mark) => {
    return mark.style.backgroundPosition === '0px -52px';
  });
  return marks.length > 0;
}
