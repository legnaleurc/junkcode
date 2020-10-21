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
        await queue.put(() => fakeTask(1, 'abc'));
      }
      await fakeTask(1);
      for (let i = 0; i < 20; ++i) {
        await queue.put(() => fakeTask(1, 'abc'));
      }
    }

    async function consume (queue) {
      while (true) {
        const task = await queue.get();
        if (!task) {
          break;
        }
        const rv = await task();
        console.info(rv);
      }
    }

    const q = new Queue();
    await Promise.all([
      produce(q),
      consume(q),
      consume(q),
      consume(q),
      consume(q),
      consume(q),
      consume(q),
    ]);
  });
});
