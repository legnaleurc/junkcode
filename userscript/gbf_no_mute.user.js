// ==UserScript==
// @name         GBF no mute
// @namespace    sandbox
// @match        http://game.granbluefantasy.jp/*
// @grant        unsafeWindow
// ==/UserScript==

'use strict';

const oael = unsafeWindow.EventTarget.prototype.addEventListener;
unsafeWindow.EventTarget.prototype.addEventListener = function (type, listener, useCapture) {
  if (type === 'blur') {
    return;
  }
  return oael.call(this, type, listener, useCapture);
};
