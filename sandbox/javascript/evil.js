(function (exports, global) {
    'use strict';

    var evil = global.evil, slice = Array.prototype.slice;

    if (exports === undefined) {
        exports = global.evil = {};
        exports.noConflict = function () {
            if (global.evil === exports) {
                global.evil = evil;
            }
            return exports;
        };
    }

    function extend (object) {
        slice.call(arguments, 1).forEach(function (source) {
            var prop;
            if (!source) {
                return;
            }
            for (prop in source) {
                if (source.hasOwnProperty(prop)) {
                    object[prop] = source[prop];
                }
            }
        });
        return object;
    }

    exports.extend = function (protoProps, staticProps) {
        var parent = this, child;

        if (protoProps && protoProps.hasOwnProperty('constructor')) {
            child = protoProps.constructor;
        } else {
            child = function () {
                return parent.apply(this, arguments);
            };
        }

        extend(child, parent, staticProps);

        child.prototype = Object.create(parent.prototype);
        child.prototype.constructor = child;

        if (protoProps) {
            extend(child.prototype, protoProps);
        }

        child.super = parent.prototype;

        return child;
    };

    exports.partial = function partial () {
        var fn = this, args = slice.call(arguments);
        return function () {
            return fn.apply(this, args.concat(slice.call(arguments)));
        };
    };

    exports.asyncForEach = function (seq, fn) {
        return seq.reduce(function (previous, current, index, array) {
            return previous.then(partial(fn, current, index, array));
        }, Promise.resolve());
    };

    exports.wait = function (msDelay) {
        return new Promise(function (resolve, reject) {
            setTimeout(resolve, msDelay);
        });
    };

    exports.asyncWhile = function asyncWhile (condition, fn) {
        if (!condition()) {
            return Promise.resolve();
        }
        return fn().then(partial(asyncWhile, condition, fn));
    };

    function subcoro (g, rv) {
      var next = g.next(rv);
      var p = next.value;
      if (!next.done) {
        return Promise.resolve(p);
      }
      return p.then((rv) => {
        return subcoro(g, rv);
      });
    }

    exports.coro = function (gfn) {
      var g = gfn();
      return subcoro(g);
    };

}(((typeof exports === 'undefined') ? undefined : exports), this));

// ex: ts=4 sts=4 sw=4 et
// kate: space-indent on; indent-width 4; mixedindent off; replace-tabs on;
// sublime: tab_size 4; translate_tabs_to_spaces true; detect_indentation false; use_tab_stops true;
