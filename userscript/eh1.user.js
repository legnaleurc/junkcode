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
    if (hasFlags(item) || hasCategories(item)) {
      item.style.opacity = 0.2;
    }
  });
}


function hasFlags (item) {
  let marks = item.querySelectorAll('.tft');
  marks = Array.prototype.filter.call(marks, (mark) => {
    return [
      'female:netorare',
      'parody:touhou project',
    ].some(_ => _ === mark.title);
  });
  return marks.length > 0;
}


function hasCategories (item) {
  let marks = item.querySelectorAll('.id41');
  marks = Array.prototype.filter.call(marks, (mark) => {
    return mark.title === 'Western';
  });
  return marks.length > 0;
}
