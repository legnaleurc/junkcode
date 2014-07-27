(function () {
	'use strict';

	var USERNAME = '';
	var PASSWORD = '';

	document.addEventListener('DOMContentLoaded', function () {
		var s = $('script:not([src]):contains("https://www.dmm.com/my/-/login/ajax-get-token/")').text();

		var dmm_token = s.match(/"DMM_TOKEN", "([^"]+)"/);
		dmm_token = dmm_token[1];
		var token = s.match(/"token": "([^"]+)"/);
		token = token[1];

		$.ajax({
			url: 'https://www.dmm.com/my/-/login/ajax-get-token/',
			beforeSend: function( xhr ) {
				xhr.setRequestHeader("DMM_TOKEN", dmm_token);
			},
			type: "POST",
			async: false,
			data: {
				"token": token,
			},
			dataType: "json",
			success: function (data) {
				var token = data.token;
				var idKey = data.login_id;
				var pwKey = data.password;

				$('#login_id').val(USERNAME);
				$('#password').val(PASSWORD);
				$("#id_token").attr("value", token);
				$("form").append($("<input>", {
					type: "hidden",
					name: idKey,
					value: USERNAME,
				})).append($("<input>", {
					type: "hidden",
					name: pwKey,
					value: PASSWORD,
				})).submit();
			},
			error: function (res) {
				console.error("通信エラーが発生しました");
				return false;
			},
		});
	});
})();
