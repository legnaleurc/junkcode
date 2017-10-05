browser.menus.create({
  id: 'send-to-transmission',
  title: 'Send To Transmission',
  contexts: ['link'],
});

browser.menus.onClicked.addListener((info, tab) => {
  if (info.menuItemId === 'send-to-transmission') {
    sendToTransmission(info.linkUrl)
      .then((rv) => {
        return showPrompt(tab.id, 'ok');
      })
      .catch((e) => {
        console.error('Download with Transmission', e);
        return showPrompt(tab.id, e.toString());
      });
  }
});
