import chai from 'chai';

import {
  map,
} from '../lib/index';

chai.should();


describe('sync', () => {

  describe('map', () => {

    it('should accept Array', () => {
      let a = [1, 2, 3];
      a = map(a, v => v * 2);
      a = Array.from(a);
      a.should.equal([2, 4, 6]);
    });

  });

});
