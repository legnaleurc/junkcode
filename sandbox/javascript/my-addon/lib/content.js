function wait (msDelay) {
  return new Promise((resolve) => {
    setTimeout(resolve, msDelay);
  });
}


function makePrompt (message) {
  const block = document.createElement('div');
  block.classList.add('download-with-transmission', 'bubble');
  block.textContent = message;
  block.addEventListener('transitionend', () => {
    block.parentElement.removeChild(block);
  });
  return block;
}


async function showPrompt (message) {
  const anchor = document.activeElement;
  const block = makePrompt(message);
  document.body.appendChild(block);

  const position = getTargetPosition(anchor, block);
  moveElementCenterTo(block, position);
  block.classList.add('fade');
}


function getTargetPosition (origin, block) {
  origin = toCenterFromTopLeft(origin.getBoundingClientRect());
  block = toCenterFromTopLeft(block.getBoundingClientRect());
  // TODO top only for now
  return {
    x: origin.x,
    y: origin.y - origin.height / 2 - block.height / 2,
    width: block.width,
    height: block.height,
  };
}


function toCenterFromTopLeft (rect) {
  return {
    x: rect.left + rect.width / 2,
    y: rect.top + rect.height / 2,
    width: rect.width,
    height: rect.height,
  };
}


function toTopLeftFromCenter (rect) {
  return {
    left: rect.x - rect.width / 2,
    top: rect.y - rect.height / 2,
    width: rect.width,
    height: rect.height,
  };
}


function moveElementCenterTo (element, position) {
  position = toTopLeftFromCenter(position);
  element.style.top = position.top + 'px';
  element.style.left = position.left + 'px';
}


browser.runtime.onMessage.addListener((message, sender, sendResponse) => {
  if (message.topic !== 'show-prompt') {
    return;
  }
  return showPrompt(message.args.message);
});
