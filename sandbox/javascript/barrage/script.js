(function () {
    'use strict';

    videojs.options.flash.swf = "video-js.swf";

    videojs('#video');

    var a = setInterval(function () {
        $('#barrage').barrage('add', 'hello');
    }, 1000);
})();
