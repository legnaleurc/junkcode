// ==UserScript==
// @name     Block YouTube
// @version  1
// @grant    none
// @match    https://www.youtube.com/watch*
// ==/UserScript==

'use strict';


function main () {
  remove('#header');
  remove('#watch7-sidebar-ads');
  remove('.video-ads');
  remove('#pyv-watch-related-dest-url');
  if (document.querySelector('.videoAdUiRedesign')) {
    let el = document.querySelector('.video-stream');
    el.src = '';
  }
}


function remove (selector) {
  const els = document.querySelectorAll(selector);
  els.forEach((el) => {
    el.remove();
  });
}


setInterval(main, 1000);
