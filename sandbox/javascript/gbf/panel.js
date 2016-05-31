// This one acts in the context of the panel in the Dev Tools
//
// Can use
// chrome.devtools.*
// chrome.extension.*

'use strict';

var gRunButton = document.querySelector('#btn-run');

gRunButton.addEventListener('click', function (event) {
  sendObjectToInspectedPage({
    action: 'code',
    content: 'console.info("asfjkalkefjaweoifj")',
  });
});
