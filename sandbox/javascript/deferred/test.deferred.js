

    var should = chai.should();
    var expect = chai.expect;

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

    it('can create read-only promise', function (done) {
        var d = new Deferred();
        var p = d.promise();
        setTimeout(function () {
            d.resolve();
        }, 10);
        p.done(function () {
            done();
        });
    });

    it('can resolve with multiple parameters', function (done) {
        var d = new Deferred();
        setTimeout(function () {
            d.resolve(1, 2, 3);
        }, 10);
        d.done(function (a, b, c) {
            a.should.equals(1);
            b.should.equals(2);
            c.should.equals(3);
            done();
        });
    });

});
