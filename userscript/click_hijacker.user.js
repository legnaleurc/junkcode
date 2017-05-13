// ==UserScript==
// @name        click hijacker
// @namespace   sandbox
// @include     http://*
// @include     https://*
// @version     1
// @grant       unsafeWindow
// @run-at      document-start
// ==/UserScript==

'use strict';

var oael = unsafeWindow.EventTarget.prototype.addEventListener;

unsafeWindow.EventTarget.prototype.addEventListener = exportFunction(function (type, handler, useCapture) {
  if (type === 'click') {
    if (this === document.body) {
      console.trace();
    }
    console.info('binding click on', this, handler, document.body === this);
    return oael.call(this, type, exportFunction(function (event) {
      console.info('vvvv');
      console.info('target', event.target);
      console.info('currentTarget', event.currentTarget);
      console.info('relatedTarget', event.relatedTarget);
      console.info('explicitOriginalTarget', event.explicitOriginalTarget);
      console.info('originalTarget', event.originalTarget);
      console.info('handler', handler.toString());
      console.info('^^^^');
      // debugger;
      // handler;
    }, unsafeWindow, {
      allowCrossOriginArguments: true,
    }), useCapture);
  }
  return oael.call(this, type, handler, useCapture);
}, unsafeWindow, {
  allowCrossOriginArguments: true,
});


unsafeWindow.setInterval = function (handler, time) {
  console.info(handler, time);
  console.trace();
};
