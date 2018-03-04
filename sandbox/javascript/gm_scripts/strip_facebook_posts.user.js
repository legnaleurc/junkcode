// ==UserScript==
// @name        Strip Facebook Posts
// @namespace   sandbox
// @run-at      document-start
// @grant       none
// @match       https://www.facebook.com/*
// @exclude     https://www.facebook.com/ajax/*
// @exclude     https://www.facebook.com/ai.php
// @version     1
// ==/UserScript==

(function () {
    'use strict';

    if (!MutationObserver) {
        if (WebKitMutationObserver) {
            // Chrome < 26
            var MutationObserver = WebKitMutationObserver;
        }
    }

    document.addEventListener('DOMContentLoaded', function () {
        var target = document.querySelector('#home_stream');
        if (!target) {
            return;
        }
        var observer = new MutationObserver(function (mutations) {
            mutations.forEach(function (mutation) {
                Array.prototype.forEach.call(mutation.addedNodes, function (child) {
                    var i = child.querySelector('span.fwn.fcg:not(.timestamp), span._6_r');
                    if (i && /suggested p(ost|age)/i.test(i.textContent)) {
                        i = child.querySelector('div.actorName a, span._706 a');
                        console.info('removed suggested post/page: ' + i.textContent);
                        target.removeChild(child);
                    }
                });
            });
        });
        var config = {
            childList: true,
        };
        observer.observe(target, config);
    });
}());

// ex: ts=4 sts=4 sw=4 et
// sublime: tab_size 4; translate_tabs_to_spaces true; detect_indentation false; use_tab_stops true;
