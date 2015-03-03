(function (global, factory) {
    global.Deferred = factory(global, global.Promise);
})(this, function (global, Promise) {
    'use strict';

    function Promise_ (deferred) {
        this._deferred = deferred;
    }

    Promise_.prototype.done = function (onFulfilled) {
        this._deferred.done(onFulfilled);
        return this;
    };

    Promise_.prototype.fail = function (onRejected) {
        this._deferred.fail(onRejected);
        return this;
    };

    function Deferred () {
        var self = this;
        this._promise = new Promise(function (resolve, reject) {
            self._resolve = resolve;
            self._reject = reject;
        });
    }

    Deferred.prototype.done = function (onFulfilled) {
        this._promise.then(function (args) {
            var nexus = onFulfilled.apply(undefined, args);
            if (nexus instanceof Deferred || nexus instanceof Promise_) {
                return nexus._promise;
            } else {
                return nexus;
            }
        }, undefined);
        return this;
    };

    Deferred.prototype.fail = function (onRejected) {
        this._promise.then(undefined, function (args) {
            var nexus = onRejected.apply(undefined, args);
            if (nexus instanceof Deferred || nexus instanceof Promise_) {
                return nexus._promise;
            } else {
                return nexus;
            }
        });
        return this;
    };

    Deferred.prototype.promise = function () {
        return new Promise_(this);
    };

    Deferred.prototype.resolve = function () {
        var args = Array.prototype.slice.call(arguments);
        this._resolve(args);
        return this;
    };

    Deferred.prototype.reject = function () {
        var args = Array.prototype.slice.call(arguments);
        this._reject(args);
        return this;
    };

    return Deferred;
});
