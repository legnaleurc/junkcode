export function * filter (iterable, fn) {
  for (const v of iterable) {
    if (fn(v)) {
      yield v;
    }
  }
}

export function * map (iterable, fn) {
  for (const v of iterable) {
    yield fn(v);
  }
}
