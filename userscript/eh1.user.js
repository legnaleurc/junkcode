// ==UserScript==
// @name        eh1
// @namespace   sandbox
// @include     https://exhentai.org/*
// @include     https://e-hentai.org/*
// @version     1
// @grant       none
// ==/UserScript==

'use strict';

main();


async function main () {
  const items = document.querySelectorAll('.id1');

  items.forEach((item) => {
    let marks = item.querySelectorAll('.tft');
    marks = Array.prototype.filter.call(marks, (mark) => {
      return mark.title === 'female:netorare';
    });
    if (marks.length <= 0) {
      marks = item.querySelectorAll('.id41');
      marks = Array.prototype.filter.call(marks, (mark) => {
        return mark.title === 'Western';
      });
      if (marks.length <= 0) {
        return;
      }
    }

    item.style.opacity = 0.2;
  });
}
