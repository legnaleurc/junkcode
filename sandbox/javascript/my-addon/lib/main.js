const {Cc, Ci} = require('chrome');
var ww = Cc["@mozilla.org/embedcomp/window-watcher;1"].getService(Ci.nsIWindowWatcher);

ww.registerNotification({
  observe: function (aSubject, aTopic, aData) {
    if (aTopic === 'domwindowopened') {
      aSubject.addEventListener("load", function (event) {
        var w = event.currentTarget;
        if (w.location.toString() !== 'chrome://greasemonkey/content/install.xul') {
          return;
        }
        var xul = w.document.documentElement;
        var install = xul.getButton('accept');
        if (!install) {
          return;
        }
        var i = w.setInterval(function () {
          if (install.disabled) {
            return;
          }
          w.clearInterval(i);
          install.click();
        }, 500);
      });
    }
  },
});
