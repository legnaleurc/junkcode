export class Queue {

  constructor () {
    this._unfinished = 0;
    this._list = [];
    this._getterList = [];
    this._finished = new FakePromise();
  }

  get isEmpty () {
    return this._list.length <= 0;
  }

  get isFull () {
    return false;
  }

  async put (task) {
    this._list.push(task);
    this._unfinished += 1;
    this._wakeup(this._getterList);
  }

  async get () {
    while (this.isEmpty) {
      const getter = new FakePromise();
      this._getterList.push(getter);
      await getter.promise;
    }
    const top = this._list.shift();
    return top;
  }

  async taskDone () {
    if (this._unfinished <= 0) {
      throw new Error('taskDone() called too many times');
    }
    this._unfinished -= 1;
    if (this._unfinished === 0) {
      this._finished.resolve();
    }
  }

  async join () {
    if (this._unfinished > 0) {
      await this._finished.promise;
    }
  }

  _wakeup (waiterList) {
    while (waiterList.length > 0) {
      const waiter = waiterList.shift();
      waiter.resolve();
      break;
    }
  }

}


class FakePromise {

  constructor () {
    this._resolve = null;
    this._reject = null;
    this._promise = new Promise((resolve, reject) => {
      this._resolve = resolve;
      this._reject = reject;
    });
  }

  resolve (rv = undefined) {
    this._resolve(rv);
  }

  reject (rv = undefined) {
    this._reject(rv);
  }

  get promise () {
    return this._promise;
  }

}
