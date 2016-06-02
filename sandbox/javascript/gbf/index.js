(function () {
  'use strict';

  function main () {
    var tmp = location.hash.substr(1);
    tmp = loadFromBase64(tmp);
    console.info(tmp);
  }

  function loadFromBase64 (b64) {
    var json = atob(b64);
    json = escape(json);
    json = decodeURIComponent(json);
    json = JSON.parse(json);
    return json;
  }

  main();

})();
