export function * map (iterable, fn) {
  for (const v of iterable) {
    yield fn(v);
  }
}
