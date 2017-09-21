import chai from 'chai';

import {
  filter,
  map,
} from '../lib/index';

chai.should();


describe('sync', () => {

  describe('filter', () => {

    it('should accept Array', () => {
      let a = [1, 2, 3];
      a = filter(a, v => v % 2 === 1);
      a = Array.from(a);
      a.should.deep.equal([1, 3]);
    });

    it('should accept Map', () => {
      let a = new Map();
      a.set('a', 1);
      a.set('b', 2);
      a.set('c', 3);
      a = filter(a, ([k, v]) => k === 'c' || v % 2 === 0);
      a = new Map(a);
      let b = new Map();
      b.set('b', 2);
      b.set('c', 3);
      a.should.deep.equal(b);
    });

  });

  describe('map', () => {

    it('should accept Array', () => {
      let a = [1, 2, 3];
      a = map(a, v => v * 2);
      a = Array.from(a);
      a.should.deep.equal([2, 4, 6]);
    });

    it('should accept Map', () => {
      let a = toMap({
        a: 1,
        b: 2,
        c: 3,
      });
      a = map(a, ([k, v]) => [k + k, v * 2]);
      a = new Map(a);
      let b = toMap({
        aa: 2,
        bb: 4,
        cc: 6,
      });
      a.should.deep.equal(b);
    });

    it('should be able to use as pipe', () => {
      let a = [1, 2, 3];
      a = map(a, v => v * 2);
      a = map(a, v => v * 3);
      a = Array.from(a);
      a.should.deep.equal([6, 12, 18]);
    });

  });

});


function toMap (object) {
  return new Map(Object.entries(object));
}
