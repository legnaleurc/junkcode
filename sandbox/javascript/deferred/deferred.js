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
        this._resolve = null;
        this._reject = null;
        var self = this;
        this._promise = new Promise(function (resolve, reject) {
            self._resolve = resolve;
            self._reject = reject;
        });
    }

    Deferred.prototype.done = function (onFulfilled) {
        this._promise.then(onFulfilled);
        return this;
    };

    Deferred.prototype.fail = function (onRejected) {
        this._promise.then(undefined, onRejected);
        return this;
    };

    Deferred.prototype.promise = function () {
        return new Promise_(this);
    };

    Deferred.prototype.resolve = function () {
        this._resolve();
        return this;
    };

    Deferred.prototype.reject = function () {
        this._reject();
        return this;
    };

    return Deferred;
});
