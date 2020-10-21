import chai from 'chai';

import { Queue } from '../src/queue';


describe('queue', () => {
  it('producer-consumer test', async () => {
    function fakeTask (msDelay, rv) {
      return new Promise((resolve) => {
        const handle = setTimeout(() => {
          resolve(rv);
        }, msDelay);
      });
    }

    async function produce (queue) {
      for (let i = 0; i < 10; ++i) {
        await queue.put(() => fakeTask(1, 'p1'));
      }
      await fakeTask(1);
      for (let i = 0; i < 20; ++i) {
        await queue.put(() => fakeTask(1, 'p2'));
      }
      await queue.put(null);
      await queue.put(null);
      await queue.put(null);
      await queue.put(null);
      await queue.put(null);
      await queue.put(null);
    }

    async function consume (queue, id) {
      while (true) {
        const task = await queue.get();
        if (!task) {
          break;
        }
        const rv = await task();
        console.info(id, rv);
      }
    }

    const q = new Queue();
    await Promise.all([
      produce(q),
      consume(q, 'c1'),
      consume(q, 'c2'),
      consume(q, 'c3'),
      consume(q, 'c4'),
      consume(q, 'c5'),
      consume(q, 'c6'),
    ]);
  });
});
