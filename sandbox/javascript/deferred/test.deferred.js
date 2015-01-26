describe('Deferred', function () {
    'use strict';

    it('can be resolved', function (done) {
        var d = new Deferred();
        setTimeout(function () {
            d.resolve();
        }, 10);
        d.done(function () {
            done();
        });
    });

    it('can be rejected', function (done) {
        var d = new Deferred();
        setTimeout(function () {
            d.reject();
        }, 10);
        d.fail(function () {
            done();
        });
    });

});
