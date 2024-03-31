import type { Action } from "svelte/action";

type EffectFunction<N, P> = (node: N, params: P) => () => void;

export function makeAction<N, P>(
  setEffect: EffectFunction<N, P>,
): Action<N, P> {
  return (node: N, params?: P) => {
    let clear = setEffect(node, params!);
    return {
      update(newParams: P) {
        clear();
        clear = setEffect(node, newParams);
      },
      destroy() {
        clear();
      },
    };
  };
}
