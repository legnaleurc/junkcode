export function * map (iterable, fn) {
  for (const v in iterable) {
    yield fn(v);
  }
}
