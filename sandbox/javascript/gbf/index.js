(function () {
  'use strict';

  function main () {
    var jobFields = new Backbone.Collection([
      {
        name: 'rank',
        comment: 'Rank lv.',
      },
    ]);
    var jobStatus = new GBF.JobTable({
      collection: jobFields,
      el: '#hero-status',
    });

    jobStatus.render();

    var tmp = location.hash.substr(1);
    if (tmp) {
      tmp = loadFromBase64(tmp);
    }
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
