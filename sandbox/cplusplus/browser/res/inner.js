(function () {
	'use strict';

	document.addEventListener('DOMContentLoaded', function () {
		var flashWrap = document.querySelector('#flashWrap');
		var observer = new MutationObserver(function (mutations) {
			mutations.filter(function (mutation) {
				return mutation.type === 'childList';
			}).forEach(function (mutation) {
				Array.prototype.forEach.call(mutation.addedNodes, function (node) {
					if (node.id === 'externalswf') {
						observer.disconnect();
						var body = document.createElement('body');
						document.body = body;
						document.body.appendChild(node);
						var html = document.documentElement;
						html.style.overflow = 'hidden';
						html.style.webkitUserSelect = 'none';
						html.style.width = '800px';
						html.style.height = '480px';
						body.style.width = '800px';
						body.style.height = '480px';
					}
				});
			});
		});
		observer.observe(flashWrap, {
			childList: true,
			subtree: true,
		});
	});
})();
