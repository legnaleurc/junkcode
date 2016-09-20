chrome.contextMenus.create({
  id: 'download',
  title: 'Download',
  documentUrlPatterns: ['https://exhentai.org/g/*'],
  contexts: ['link'],
});

chrome.contextMenus.onClicked.addListener(function(info, tab) {
  if (info.menuItemId === 'download') {
    chrome.tabs.executeScript(tab.id, {
      code: '[document.querySelector(\'#gj\').textContent, document.querySelector(\'p.g2:nth-child(2) > a:nth-child(2)\').onclick.toString()]',
    }, function (result) {
      if (!result || !result[0]) {
        return;
      }
      let [name, url] = result[0];
      url = url.match(/'([^']+)'/);
      if (!url) {
        return;
      }
      url = url[1];


      var xhr = new XMLHttpRequest();
      xhr.open("GET", url, true);
      xhr.onreadystatechange = function() {
        if (xhr.readyState == 4) {
          let url = xhr.responseText.match(/a href="([^"]+)"/);
          if (!url) {
            return;
          }
          url = url[1] + '?start=1';

          chrome.downloads.download({
            url: url,
            filename: name + '.zip',
          }, function (downloadId) {
            console.info(downloadId);
          });
        }
      }
      xhr.send();

      // fetch(info.linkUrl).then(function (data) {
      //   console.info(data);
      // });
    });
  }
});
