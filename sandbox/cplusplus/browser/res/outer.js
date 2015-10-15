(function () {
	'use strict';

	document.addEventListener('DOMContentLoaded', function () {
		var game_frame = document.querySelector('#game_frame');
		window.top.location.replace(game_frame.src);
	});
})();
