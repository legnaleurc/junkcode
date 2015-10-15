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

    function asyncForEach (seq, index, fn) {
        if (index >= seq.length) {
            return Promise.resolve();
        }
        return fn(seq[index], index, seq).then(asyncForEach.bind(this, seq, index + 1, fn));
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

    exports.partial = function () {
        var fn = this, args = slice.call(arguments);
        return function () {
            return fn.apply(this, args.concat(slice.call(arguments)));
        };
    };

    exports.asyncForEach = function (seq, fn) {
        return asyncForEach(seq, 0, fn);
    };

}(((typeof exports === 'undefined') ? undefined : exports), this));

// ex: ts=4 sts=4 sw=4 et
// kate: space-indent on; indent-width 4; mixedindent off; replace-tabs on;
// sublime: tab_size 4; translate_tabs_to_spaces true; detect_indentation false; use_tab_stops true;
