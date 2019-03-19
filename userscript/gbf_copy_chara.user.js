// ==UserScript==
// @name         GBF copy chara
// @namespace    sandbox
// @version      0.1
// @match        http://game.granbluefantasy.jp/*
// @grant        none
// ==/UserScript==

'use strict';

document.addEventListener('copy', (event) => {
    event.preventDefault();

    let chara = document.querySelector('#char1');
    if (!chara) {
        return;
    }
    chara = chara.src;
    event.clipboardData.setData('text/plain', chara);
});
