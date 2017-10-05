function wait (msDelay) {
  return new Promise((resolve) => {
    setTimeout(resolve, msDelay);
  });
}


async function showPrompt (message) {
  const position = document.activeElement.getBoundingClientRect();

  const block = document.createElement('div');
  block.classList.add('download-with-transmission', 'bubble');
  block.textContent = message;

  document.body.appendChild(block);

  block.style.top = position.top;
  block.style.left = position.left;

  // await wait(5000);
  // block.parentElement.removeChild(block);
}


browser.runtime.onMessage.addListener((message, sender, sendResponse) => {
  if (message.topic !== 'show-prompt') {
    return;
  }
  return showPrompt(message.args.message);
});
