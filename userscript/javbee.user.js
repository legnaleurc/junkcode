// ==UserScript==
// @name     JavBee
// @version  1
// @grant    none
// @match    https://*.javbee.vip/*
// ==/UserScript==


function handleImageLinks() {
  const linkList = document.querySelectorAll('a.img-items');
  linkList.forEach((a) => {
    const url = a.textContent.trim();
    a.href = url;
  });
}


handleImageLinks();
