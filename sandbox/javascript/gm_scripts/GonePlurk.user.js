// ==UserScript==
// @name        GonePlurk
// @namespace   sandbox
// @include     http://www.plurk.com/*
// @version     1
// ==/UserScript==

(function () {
  'use strict';
  window.Media._hideLink = function (a) {
    window.open(a, '_blank');
  };
}());

// sublime: tab_size 2; translate_tabs_to_spaces true; detect_indentation false; use_tab_stops true;
