(function (exports) {
    'use strict';

    var slice = Array.prototype.slice;

    function extend(object) {
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
        var parent = this, child, Surrogate;

        if (protoProps && protoProps.hasOwnProperty('constructor')) {
            child = protoProps.constructor;
        } else {
            child = function () {
                return parent.apply(this, arguments);
            };
        }

        extend(child, parent, staticProps);

        Surrogate = function () {
            this.constructor = child;
        };
        Surrogate.prototype = parent.prototype;
        child.prototype = new Surrogate();

        if (protoProps) {
            extend(child.prototype, protoProps);
        }

        child.super = parent.prototype;

        return child;
    };

    exports.currying = function () {
        var fn = this, args = slice.call(arguments);
        return function () {
            return fn.apply(this, args.concat(slice.call(arguments)));
        };
    };
}((typeof exports === 'undefined') ? (this.evil = {}) : exports));

// ex: ts=4 sts=4 sw=4 et
// kate: space-indent on; indent-width 4; mixedindent off; replace-tabs on;
// sublime: tab_size 4; translate_tabs_to_spaces true; detect_indentation false; use_tab_stops true;
