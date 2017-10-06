browser.menus.create({
  id: 'send-to-transmission',
  title: 'Send To Transmission',
  contexts: ['link'],
});

browser.menus.onClicked.addListener((info, tab) => {
  if (info.menuItemId === 'send-to-transmission') {
    main(info.linkUrl, tab.id);
  }
});


async function main (linkUrl, tabId) {
  const prompt = new PromptServer(tabId);
  await prompt.setMessage('Sending ...');
  let msg = 'Done';
  try {
    await sendToTransmission(linkUrl);
  } catch (e) {
    console.error('Download with Transmission', e);
    msg = e.toString();
  }
  await prompt.setMessage(msg);
  await prompt.close();
}
