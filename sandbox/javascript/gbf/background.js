'use strict';

function extensionListener (port, message, sender, sendResponse) {
  if(message.tabId && message.content) {
    if(message.action === 'code') {
      //Evaluate script in inspectedPage
      chrome.tabs.executeScript(message.tabId, {
        code: message.content,
      });
    } else if(message.action === 'script') {
      //Attach script to inspectedPage
      chrome.tabs.executeScript(message.tabId, {
        file: message.content,
      });
    } else {
      //Pass message to inspectedPage
      chrome.tabs.sendMessage(message.tabId, message, sendResponse);
    }
  } else {
    // This accepts messages from the inspectedPage and
    // sends them to the panel
    port.postMessage(message);
  }
  sendResponse(message);
}


chrome.extension.onConnect.addListener(function (port) {
  // Listens to messages sent from the panel
  chrome.extension.onMessage.addListener(extensionListener.bind(null, port));
  port.onDisconnect.addListener(function(port) {
    chrome.extension.onMessage.removeListener(extensionListener);
  });
});


chrome.runtime.onMessage.addListener(function(request, sender, sendResponse) {
  return true;
});
