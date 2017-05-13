// ==UserScript==
// @name        skidrow_blocker
// @namespace   AdsBypasser
// @description Block something on Skidrow
// @copyright   2014+, Wei-Cheng Pan (legnaleurc)
// @version     1
// @license     MIT
// @grant       unsafeWindow
// @include     http://skidrowcrack.com/*
// @run-at      document-start
// ==/UserScript==

(function () {
  'use strict';

  function wrap (fn) {
    if (typeof exportFunction !== 'function') {
      return fn;
    }
    return exportFunction(fn, unsafeWindow, {
      allowCrossOriginArguments: true,
    });
  }

  var nop = wrap(function () {
    console.info(arguments);
  });

  unsafeWindow.open = nop;

  document.addEventListener('DOMContentLoaded', function () {
    unsafeWindow.document.body.addEventListener = nop;
  });

})();
