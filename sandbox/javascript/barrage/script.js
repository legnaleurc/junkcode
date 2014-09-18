(function () {
    'use strict';

    videojs.options.flash.swf = "video-js.swf";

    // var a = setInterval(function () {
    //     $('#barrage').barrage('add', 'hello');
    // }, 1000);

    var v = videojs('#video');
    var $input = $('#input');
    var $barrage = $('#barrage').barrage();
    var $history = $('#history');

    function addComment (comment) {
        $barrage.barrage('add', comment);
        comment = $.parseHTML(comment);
        comment = $(comment).text();
        comment = $.parseHTML('<div><p>' + comment + '</p></div>');
        comment = $(comment);
        $history.append(comment);
        $history.scrollTop(100000);
    }

    $.get('/cgi-bin/tunnel.cgi').then(function (data) {
        v.src({
            src: data,
            type: 'rtmp/mp4',
        });
        v.play();
    });

    $(window).on('keydown', function (event) {
        if ($input.is(':focus')) {
            return;
        }

        var which = event.which;
        var modifier = event.metaKey || event.ctrlKey;

        if (!modifier) {
            return;
        }

        switch (which) {
        case 37:
            console.info('left');
            event.preventDefault();
            break;
        case 39:
            console.info('right');
            event.preventDefault();
            break;
        default:
            ;
        }
    });

    $input.on('keydown', function (event) {
        var which = event.which;
        if (which !== 10 && which !== 13) {
            return;
        }

        var text = $input.val();
        $input.val('');

        socket.emit('comment', text);
        addComment(text);
    });

    var socket = io('http://localhost:9527');
    socket.on('comment', function (data) {
        addComment(data);
    });

})();
