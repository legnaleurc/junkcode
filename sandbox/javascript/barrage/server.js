var IRC = require( 'irc' );
var SocketIO = require( 'socket.io' );

var CHANNEL = '#sitcon';
var NICK = 'comment';
var PORT = 9527;

var io = SocketIO(PORT);

io.on('connection', function (socket) {
    socket.on('comment', function (data) {
        socket.broadcast.emit('comment', data);
    });
} );

// var client = new IRC.Client( 'irc.freenode.net', NICK, {
//     channels: [
//     CHANNEL,
//     ],
// } );

// client.addListener( 'message', function( nick, to, text ) {
//     var pattern = new RegExp( '^' + NICK + '[,:]\\s*(.*)\\s*$' );
//     pattern = text.match( pattern );
//     if( to === CHANNEL && pattern ) {
//         text = pattern[1];
//     } else if( to !== NICK ) {
//         return;
//     }
//     io.sockets.in( CHANNEL ).emit( 'comment', text );
// } );
// client.addListener( 'error', function( msg ) {
//     console.log( msg );
// } );
