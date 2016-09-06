chrome.contextMenus.create({
  id: 'send-to-transmission',
  title: 'Send To Transmission',
  contexts: ['link'],
});

chrome.contextMenus.onClicked.addListener(function(info, tab) {
  if (info.menuItemId === 'send-to-transmission') {
    var url = btoa(info.linkUrl);
    console.info(url);
  }
});
