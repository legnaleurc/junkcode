define('view', [], function () {
  'use strict';
  
  return {
    createEntry: function createEntry (entry) {
      var content = document.querySelector('#content');
      var div = document.createElement('div');
      var img = document.createElement('img');
      img.src = entry.data.coverURL;
      div.appendChild(img);
      content.appendChild(div);
    },
  };
});
