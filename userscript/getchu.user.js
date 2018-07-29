// ==UserScript==
// @name     getchu copy
// @version  1
// @grant    none
// @include  http://www.getchu.com/soft.phtml?*
// ==/UserScript==

'use strict';

document.addEventListener('copy', (event) => {
  event.preventDefault();

  let title = document.querySelector('#soft-title');
  title = title.childNodes[0];
  title = title.nodeValue.trim();

  let brand = document.querySelector('#brandsite');
  brand = brand.textContent.trim();

  let date = document.querySelector('#tooltip-day');
  date = date.textContent.trim();
  date = date.match(/(\d{4})\/(\d{2})\/(\d{2})/);
  date = `${date[1].substr(2)}${date[2]}${date[3]}`;

  const name = `[${date}] [${brand}] ${title}`;
  event.clipboardData.setData('text/plain', name);
});
