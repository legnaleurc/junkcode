export function * filter (iterable, fn) {
  if (isAsyncFunction(fn)) {
    throw new TypeError('async function is not allowed in filter');
  }
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


const AsyncFunctionConstructor = (async () => {}).constructor;


function isAsyncFunction (fn) {
  return fn instanceof AsyncFunctionConstructor;
}
