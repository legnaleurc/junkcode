class PromptClient {

  constructor () {
    this._anchor = null;
    this._block = null;
  }

  async setMessage (message) {
    this._maybeInitialize();
    this._block.textContent = message;
    this._update();
  }

  close () {
    return new Promise((resolve) => {
      this._block.addEventListener('transitionend', () => {
        this._block.parentElement.removeChild(this._block);
        this._block = null;
        this._anchor = null;
        resolve();
      });
      this._block.classList.add('fade');
    });
  }

  _maybeInitialize () {
    if (!this._anchor) {
      this._anchor = document.activeElement;
    }
    if (!this._block) {
      this._block = this._create();
      document.body.appendChild(this._block);
    }
  }

  _create () {
    const block = document.createElement('div');
    block.classList.add('download-with-transmission', 'bubble');
    return block;
  }

  _update () {
    const position = getTargetPosition(this._anchor, this._block);
    moveElementCenterTo(this._block, position);
  }

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


const prompt = new PromptClient();

browser.runtime.onMessage.addListener((message, sender, sendResponse) => {
  switch (message.topic) {
    case 'show-prompt':
      return prompt.setMessage(message.args.message);
    case 'close-prompt':
      return prompt.close();
    default:
      break;
  }
});
