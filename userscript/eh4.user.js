// ==UserScript==
// @name     eh style
// @version  1
// @grant    none
// @include  https://exhentai.org/*
// ==/UserScript==


function main () {
  if (location.pathname.indexOf('/mpv/') === 0) {
    modifyMultiPageViewer();
  }
}


function modifyMultiPageViewer () {
  addStyle(`
    div.mi0 {
      min-width: unset;
    }
  `);

  setInterval(() => {
    let ai = document.querySelectorAll('#pane_images_inner .mi0 > a > img');
    ai.forEach((img) => {
      img.style.width = '100%';
      img.style.height = 'auto';
    });
  }, 2000);
}


function addStyle (text) {
  let style = document.createElement('style');
  style.type = 'text/css';
  style.textContent = text;
  document.head.appendChild(style);
}


try{
  main();
} catch (e) {
  console.info(e);
}
