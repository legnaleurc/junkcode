import { makeAction } from "$lib/fp";

type KeyboardEventHandler = (event: KeyboardEvent) => void;

export const enterdown = makeAction(
  (node: HTMLInputElement, handler: KeyboardEventHandler) => {
    function handleKeyDown(event: KeyboardEvent) {
      if (event.key !== "Enter") {
        return;
      }
      event.preventDefault();
      handler(event);
    }
    node.addEventListener("keydown", handleKeyDown);
    return () => {
      node.removeEventListener("keydown", handleKeyDown);
    };
  },
);
