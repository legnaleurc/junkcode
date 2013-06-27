// ==UserScript==
// @name           Google Direct Link
// @namespace      http://userscripts.org/users/legnaleurc
// @author         Wei-Cheng Pan
// @description    remove google search and image link redirection to speed up your browsing and hide referrer
// @include        *google.*/*
// @exclude        *userscripts.org*
// @version        1.0.0
// @run-at         document-start
// @grant          GM_getValue
// @grant          GM_setValue
// ==/UserScript==

(function () {
    'use strict';

    var ua = navigator.userAgent,
        wK = ua.toLowerCase().indexOf('webkit') > -1,
        S = location.protocol === 'https:',
        D = document.querySelector('#irc_mimg'),
        config = loadConfig();

    function loadConfig() {
        return {
            hideReferer: GM_getValue('hide_referer', true),
            newTab: GM_getValue('new_tab', true),
            showCache: GM_getValue('show_cache', false),
            directImage: GM_getValue('direct_image', false)
        };
    }

    function saveConfig(c) {
        GM_setValue('hide_referer', c.hideReferer);
        GM_setValue('new_tab', c.newTab);
        GM_setValue('show_cache', c.showCache);
        GM_setValue('direct_image', c.directImage);
        return true;
    }

    function addEvent(a, b, c) {
        if (a.addEventListener) {
            a.addEventListener(b, c, false);
        }
    }

    function removeEvent(a, b, c) {
        if (a.removeEventListener) {
            a.removeEventListener(b, c, false);
        }
    }

    function hasClass(a) {
        return Array.prototype.slice.call(arguments, 1).some(function (b) {
            return a.classList.contains(b);
        });
    }

    function hasAllClass(a) {
        return Array.prototype.slice.call(arguments, 1).every(function (b) {
            return a.classList.contains(b);
        });
    }

    if (Object.defineProperty) {
        Object.defineProperty(window, 'rwt', {
            value: function () {},
            writable: false,
            configurable: false
        });
    } else {
        window.__defineGetter__('rwt', function () {
            return function () {};
        });
    }

    if (config.showCache) {
        addEvent(window, 'DOMNodeInserted', cache);
    }

    function cache() {
        var cc = document.querySelectorAll('.vshid');
        Array.prototype.forEach.call(cc, function (c) {
            c.style.display = 'inline';
        });
    }

    if (D) {
        D.addEventListener('click', blocker, true);
    }

    var observer = new MutationObserver(function (mutations) {
        mutations.forEach(function (mutation) {
            if (mutation.type !== 'childList') {
                return;
            }
            Array.prototype.forEach.call(mutation.addedNodes, function (n) {
                if (n.nodeType !== Node.ELEMENT_NODE) {
                    return;
                }
                var t = n.querySelector('.irc_but_r.irc_but_tb');
                if (!t) {
                    return;
                }
                t.removeEventListener('click', blocker, true);
                t.addEventListener('click', blocker, true);
            });
        });
    });
    observer.observe(document, {
        childList: true,
        subtree: true
    });

    function blocker(e) {
        e.stopPropagation();
    }

    function proxy(e) {
        if (e && e.localName == 'a' && (hasClass(e, 'l', 'rg_l', 'rg_ilmn', 'irc_but') || e.id == 'rg_hl' || e.id == 'irc_mil' || hasClass(e.parentNode, 'vshid', 'gl', 'r'))) {
            var m = /(&url=([^&]+)|imgurl=([^&]+))(&w=\d+&h=\d+)?&ei/g.exec(decodeURIComponent(e.href));
            if (e.onmousedown) e.removeAttribute('onmousedown');
            if (m) e.href = m[2] || m[3];
            if (config.newTab) e.target = "_blank";
            if (config.hideReferer) {
                if (wK && !S) {
                    e.rel = "noreferrer";
                } else if (!S && e.href.indexOf('http-equiv="refresh"') == -1) {
                    e.href = 'data:text/html, <meta http-equiv="refresh" content="0;URL=' + encodeURIComponent(e.href) + '" charset="utf-8">';
                }
            }
        }
    }

    function tunnel(e, f) {
        if (e && e.localName == 'a' && (hasClass(e, 'l', 'rg_l', 'rg_ilmn', 'irc_but') || e.id == 'rg_hl' || hasClass(e.parentNode, 'vshid', 'gl', 'r'))) {
            if (e.href.indexOf('http-equiv="refresh"') > -1) {
                var rLink = /URL=([^"]+)/g.exec(decodeURIComponent(e.href));
                if (rLink) {
                    e.href = rLink[1];
                }
            }
        }
        removeEvent(f, 'mouseout', fixer);
    }

    function fixer(e) {
        var a = e.target,
            b = a;
        if (a.localName != 'a') {
            for (; a; a = a.parentNode) {
                tunnel(a, b);
            }
        } else {
            tunnel(a, b);
        }
    }

    function doStuff(e) {
        var a = e.target;
        addEvent(a, 'mouseout', fixer);
        if (config.directImage && a && a.className == 'rg_i') {
            a.removeAttribute('class');
        }
        if (a.localName != 'a') {
            for (; a; a = a.parentNode) {
                proxy(a);
            }
        } else {
            proxy(a);
        }
    }

    addEvent(window, "mousedown", doStuff);

    saveConfig(config);
}());
