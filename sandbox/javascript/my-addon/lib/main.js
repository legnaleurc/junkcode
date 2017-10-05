browser.menus.create({
  id: 'send-to-transmission',
  title: 'Send To Transmission',
  contexts: ['link'],
});

browser.menus.onClicked.addListener((info, tab) => {
  if (info.menuItemId === 'send-to-transmission') {
    sendToTransmission(info.linkUrl)
      .then((rv) => {
        console.info('after sendToTransmission', rv);
      })
      .catch((e) => {
        console.error('after sendToTransmission', e);
      });
  }
});
