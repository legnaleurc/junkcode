	window.onload = function(){
		if( 1 < window.location.search.length )
    	{
			var query = window.location.search.substring( 1 );
			importUrl(query);
    	}

		slot_update();
   		update();
	}

	var localUrl = "";
	
	var arraySummon = [
		"none",
		"attribute",
		"character",
		"magna",
		"unknown",
		"zeus"
	];

	var arrayType = [
		"off",
		"on0",
		"on1",
		"on2",
		"on3",
		"on6"
	];

	var arraySkill = [
		"none",
		"kj1",
		"kj2",
		"kj3",
		"bw3",
		"mkj1",
		"mkj2",
		"mbw3",
		"bha",
		"bhah",
		"unk1",
		"unk2",
		"str",
		"kj4",
		"bw1",
		"bw2",
		"mbw1",
		"mbw2",
		"mkm1",
		"km1",
	];

	var arraySkillHTML = [
		"none",
		"kj1",
		"kj2",
		"kj3",
		"kj4",
		"bw1",
		"bw2",
		"bw3",
		"km1",
		"mkj1",
		"mkj2",
		"mbw1",
		"mbw2",
		"mbw3",
		"mkm1",
		"bha",
		"bhah",
		"unk1",
		"unk2",
		"str",
	];

	function slot_update(){
		for (var i=0; i<10; i++) {
			var save = JSON.parse(localStorage.getItem("save_data"));
			if ( save == null ) continue;
			var slot_name = "slot" + (i+1) + "_name";
			party_name = typeof(save[slot_name]) == 'undefined' ? "" : save[slot_name];
			party_name.substr(0,10);
			document.system.save_slot.options[i].text= "SLOT" + (i+1) + " " + party_name;
		}
	}

	function update(){
		/* 基本値 */
		var rank = document.personal.rank.value;
		var job_atk = document.personal.job_atk.value;
		var master_atk = document.personal.master_atk.value;
		var ship_bonus = document.personal.ship_bonus.value;
		var attribute_bonus = document.personal.attribute_bonus.value;
		var hp_percent = document.personal.hp_percent.value;
		var basic_atk = rank * 40 + 1000;
		if ( rank < 2 ) basic_atk = 1000;
		if ( rank > 100 ) basic_atk -= (rank-100)*20;
		var showed_atk = basic_atk;
		var total_skill_percent = 0;
		var total_baha_percent = 0;
		var total_koujin_percent = 0;
		var total_magna_percent = 0;
		var total_magna_backwater = 0;
		var total_normal_backwater = 0;
		var total_unknown_power = 0;
		var total_strength_power = 0;

		var hp_coefficient = 2 * hp_percent / 100 * hp_percent / 100 - 5 * hp_percent / 100 + 3;

		/* 加護 */
		var summon_type = document.getElementsByName("summon_type");
		var summon_percent = document.getElementsByName("summon_percent");

		/* 武器 */
		var weapon_atk = document.getElementsByName("weapon_atk")
		var type_bonus = document.getElementsByName("type_bonus")
		var cosmos = document.getElementsByName("cosmos")
//		var div_adjust_atk = document.getElementsByName("adjust_atk")
		var skill_type1 = document.getElementsByName("skill_type1")
		var skill_type2 = document.getElementsByName("skill_type2")
		var skill_lv = document.getElementsByName("skill_lv")
		var div_atk_percent = document.getElementsByName("atk_percent")
		var div_internal_value = document.getElementsByName("internal_value")
		var adjust_atk = new Array();
		var baha_percent = new Array();
		var koujin_percent = new Array();
		var magna_percent = new Array();
		var normal_backwater_level = new Array();
		var magna_backwater_level = new Array();
		var unknown_power_level = new Array();
		var strength_power_level = new Array();
		
		for (i=0; i<10; i++ ) {
			var weapon_temp = eval(weapon_atk[i].value);
			if (weapon_temp == null) weapon_temp = 0;
			var fit_atk = 0;
			var cosmos_atk = 0;
			if (adjust_atk[i] == null) fit_atk = 0;
			if (type_bonus[i].value == "on0" ) fit_atk = weapon_temp * 0.2;
			if (type_bonus[i].value == "on1" ) fit_atk = weapon_temp * 0.21;
			if (type_bonus[i].value == "on2" ) fit_atk = weapon_temp * 0.23;
			if (type_bonus[i].value == "on3" ) fit_atk = weapon_temp * 0.25;
			if (type_bonus[i].value == "on6" ) fit_atk = weapon_temp * 0.3;
			if (cosmos[i].checked) cosmos_atk = weapon_temp * 0.3;
			fit_atk = Math.round(fit_atk);
			cosmos_atk = Math.round(cosmos_atk);
			adjust_atk[i] = weapon_temp + fit_atk + cosmos_atk;
//			div_adjust_atk[i].innerHTML = adjust_atk[i];
			showed_atk += adjust_atk[i];


			for (type=0; type<2; type++ ) {
				
				koujin_percent[i] = 0;
				baha_percent[i] = 0;
				magna_percent[i] = 0;
				normal_backwater_level[i] = 0;
				magna_backwater_level[i] = 0;
				unknown_power_level[i] = 0;
				strength_power_level[i] = 0;

				var skill_type_value = skill_type1[i].value;
				if (type == 0) skill_type_value = skill_type2[i].value;

				skill_level = eval(skill_lv[i].value);
				if (skill_level == 0 ) {
					skill_type_value = "none";
				} else if (skill_level > 10 ) {
					switch (skill_type_value) {
					case "kj1":
					case "kj2":
					case "kj3":
					case "kj4":
					case "bw1":
					case "bw2":
					case "bw3":
					case "km1":
					case "mkj1":
					case "mkj2":
					case "mbw1":
					case "mbw2":
					case "mbw3":
					case "mkm1":
					case "bha":
						break;
					
					default:
						skill_level = 10;
						break;
					}
				}

				switch (skill_type_value) {
				case "bha":
					if (skill_level < 10) {
						baha_percent[i] = 19 + skill_level;
					} else {
						baha_percent[i] = 30 + (skill_level - 10) * 0.4;
					}
					break;

				case "bhah":
					baha_percent[i] = 9.5 + skill_level / 2;
					if ( skill_level == 10 ) baha_percent[i] = 15;
					break;
					
				case "kj1":
					if (skill_level < 10) {
						koujin_percent[i] = 0 + skill_level;
					} else {
						koujin_percent[i] = 10 + (skill_level - 10) * 0.4;
					}
					break;

				case "kj2":
					if (skill_level < 10) {
						koujin_percent[i] = 2 + skill_level;
					} else {
						koujin_percent[i] = 12 + (skill_level - 10) * 0.5;
					}
					break;

				case "kj3":
					if (skill_level < 10) {
						koujin_percent[i] = 5 + skill_level;
					} else {
						koujin_percent[i] = 15 + (skill_level - 10) * 0.6;
					}
					break;

				case "kj4":
					if (skill_level < 10) {
						koujin_percent[i] = 6 + skill_level;
					} else {
						koujin_percent[i] = 16 + (skill_level - 10) * 0.8;
					}
					break;

				case "km1":
					if (skill_level < 10) {
						koujin_percent[i] = 0 + skill_level;
					} else {
						koujin_percent[i] = 10 + (skill_level - 10) * 0.4;
					}
					break;

				case "mkj1":
					if (skill_level < 10) {
						magna_percent[i] = 2 + skill_level;
					} else {
						magna_percent[i] = 12 + (skill_level - 10) * 0.5;
					}
					break;

				case "mkj2":
					if (skill_level < 10) {
						magna_percent[i] = 5 + skill_level;
					} else {
						magna_percent[i] = 15 + (skill_level - 10) * 0.6;
					}
					break;

				case "mkm1":
					if (skill_level < 10) {
						magna_percent[i] = 0 + skill_level;
					} else {
						magna_percent[i] = 10 + (skill_level - 10) * 0.4;
					}
					break;

				case "bw1":
					var bw_coefficient = 0;
					if ( skill_level >= 10 ) {
						bw_coefficient = 18 + (skill_level - 10) / 5 * 3;
					} else {
						bw_coefficient = -0.3 + skill_level * 1.8;
					}
					normal_backwater_level[i] = hp_coefficient * bw_coefficient / 3;
					break;

				case "bw2":
					var bw_coefficient = 0;
					if ( skill_level >= 10 ) {
						bw_coefficient = 24 + (skill_level - 10) / 5 * 6;
					} else {
						bw_coefficient = -0.4 + skill_level * 2.4;
					}
					normal_backwater_level[i] = hp_coefficient * bw_coefficient / 3;
					break;

				case "bw3":
					var bw_coefficient = 0;
					if ( skill_level >= 10 ) {
						bw_coefficient = 30 + (skill_level - 10) / 5 * 7.5;
					} else {
						bw_coefficient = -0.5 + skill_level * 3.0;
					}
					normal_backwater_level[i] = hp_coefficient * bw_coefficient / 3;
					break;

				case "mbw1":
					var bw_coefficient = 0;
					if ( skill_level >= 10 ) {
						bw_coefficient = 18 + (skill_level - 10) / 5 * 3;
					} else {
						bw_coefficient = -0.3 + skill_level * 1.8;
					}
					magna_backwater_level[i] = hp_coefficient * bw_coefficient / 3;
					break;

				case "mbw2":
					var bw_coefficient = 0;
					if ( skill_level >= 10 ) {
						bw_coefficient = 24 + (skill_level - 10) / 5 * 3;
					} else {
						bw_coefficient = -0.4 + skill_level * 2.4;
					}
					magna_backwater_level[i] = hp_coefficient * bw_coefficient / 3;
					break;

				case "mbw3":
					var bw_coefficient = 0;
					if ( skill_level >= 10 ) {
						bw_coefficient = 30 + (skill_level - 10) / 5 * 3;
					} else {
						bw_coefficient = -0.5 + skill_level * 3.0;
					}
					magna_backwater_level[i] = hp_coefficient * bw_coefficient / 3;
					break;

				case "unk1":
					unknown_power_level[i] = 2 + skill_level;
					break;

				case "unk2":
					unknown_power_level[i] = 5 + skill_level;
					break;

				case "str":
					strength_power_level[i] = 5 + skill_level;
					break;
				}

				total_baha_percent += baha_percent[i];
				total_koujin_percent += koujin_percent[i];
				total_magna_percent += magna_percent[i];
				total_normal_backwater += normal_backwater_level[i];
				total_magna_backwater += magna_backwater_level[i];
				total_unknown_power += unknown_power_level[i];
				total_strength_power += strength_power_level[i];
				
				if ( total_baha_percent > 50 ) total_baha_percent = 50;
			}
		}

		/* 召喚 */
		var summon_atk = document.getElementsByName("summon_atk")
		for (i=0; i<5; i++ ) {
			if ( eval(summon_atk[i].value) != null ) {
				showed_atk += eval(summon_atk[i].value);
			}
		}
		
		/* バフ */
		var buff_koujin    = document.personal.buff_koujin.value;
		var buff_attribute = document.personal.buff_attribute.value;
		var buff_multiply  = document.personal.buff_multiply.value;
		var total_multiply = 0;
		
		/* 結果表示 */
		/* 召喚 */
		var unknown_bonus = 100;
		var magna_bonus = 100;
		var zeus_bonus = 100;
		var attribute_percent = 100;
		var buff_koujin_percent = 0;

		if ( eval(buff_koujin) != null ) buff_koujin_percent = eval(buff_koujin);
		if ( eval(buff_attribute) != null ) attribute_percent += eval(buff_attribute);
		if ( eval(buff_multiply) != null ) total_multiply = eval(buff_multiply);

		for (var i=0; i<4; i++) {
			if ( eval(summon_percent[i].value) == null ) continue;
			if ( summon_type[i].value == "attribute" ) attribute_percent += eval(summon_percent[i].value);
			if ( summon_type[i].value == "character" ) total_skill_percent += eval(summon_percent[i].value);
			if ( summon_type[i].value == "magna" ) magna_bonus += eval(summon_percent[i].value);
			if ( summon_type[i].value == "unknown" ) unknown_bonus += eval(summon_percent[i].value);
			if ( summon_type[i].value == "zeus" ) zeus_bonus += eval(summon_percent[i].value);
		}

		if ( attribute_bonus == "good" ) attribute_percent += 50;
		if ( attribute_bonus == "bad" ) attribute_percent += -25;

		var magni_normal_koujin = total_skill_percent + total_baha_percent + (total_koujin_percent * zeus_bonus / 100) + buff_koujin_percent;
		var magni_normal_bw     = total_normal_backwater * zeus_bonus / 100;
		var magni_magna_koujin  = total_magna_percent * magna_bonus / 100;
		var magni_magna_bw      = total_magna_backwater * magna_bonus / 100;
		var magni_collabo       = total_unknown_power * unknown_bonus / 100 + total_strength_power;
		var magni_attribute     = attribute_percent - 100;

		if ( eval(job_atk) != null ) showed_atk += eval(job_atk);
		if ( eval(master_atk) != null ) showed_atk *= 1 + eval(master_atk) / 100;
		var total_atk = showed_atk;
		if ( eval(ship_bonus) != null ) total_atk *= 1 + eval(ship_bonus) / 100;

		total_atk *= attribute_percent / 100;
		total_atk *= 1 + magni_normal_koujin / 100;
		total_atk *= 1 + magni_normal_bw / 100;
		total_atk *= 1 + magni_magna_koujin / 100;
		total_atk *= 1 + magni_magna_bw / 100;
		total_atk *= 1 + total_multiply / 100;
		total_atk *= 1 + magni_collabo / 100

		showed_atk = Math.round(showed_atk);
		total_atk = Math.round(total_atk);
		document.getElementById("basic_atk").innerHTML = basic_atk;
		document.getElementById("showed_atk").innerHTML = showed_atk;
		document.getElementById("total_atk").innerHTML = total_atk;

		document.getElementById("magni_normal_koujin").innerHTML = magni_normal_koujin;
		document.getElementById("magni_normal_bw").innerHTML = magni_normal_bw;
		document.getElementById("magni_magna_koujin").innerHTML = magni_magna_koujin;
		document.getElementById("magni_magna_bw").innerHTML = magni_magna_bw;
		document.getElementById("magni_collabo").innerHTML = magni_collabo;
		document.getElementById("magni_final").innerHTML = total_multiply;
		document.getElementById("magni_attribute").innerHTML = magni_attribute;
	}

	function encodeNumber(digit, value)
	{
	if (value < 0) value = 0;
		var encodeSrting = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
		var encodeLength = encodeSrting.length;

		var ret = new String();
		var quotient = value;
		var surplus = 0;

		for (var i=0; i<digit; i++) {
			surplus = quotient % encodeLength;
			quotient = quotient / encodeLength;
			ret = encodeSrting.charAt(surplus) + ret;
		}
		
		return ret;
	}

	function decodeNumber(digit)
	{
		var encodeSrting = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
		var encodeLength = encodeSrting.length;

		var value = 0;
		if (localUrl.length < digit) return 0;
		
		for (var i=0; i<digit; i++) {
			value *= encodeLength;
			value += encodeSrting.indexOf(localUrl.substr(i, 1));
		}
		
		localUrl = localUrl.substr(digit);
		
		return value;
	}

	function outUrl() {
		var urlString = new String("http://hibin0.web.fc2.com/grbr_atk_calc/atk_calc.html?");
		urlString = urlString + createUrl();
		document.getElementById("url").value = urlString;
	}

	function createUrl() {
		var urlString = "";
		
		urlString += encodeNumber(2, 2000); // flag
		urlString += encodeNumber(2, 2); // Version

		urlString += encodeNumber(2, document.personal.rank.value);
		urlString += encodeNumber(3, document.personal.job_atk.value);
		urlString += encodeNumber(2, document.personal.master_atk.value);
		urlString += encodeNumber(2, document.personal.ship_bonus.value);
		urlString += encodeNumber(2, document.personal.attribute_bonus.selectedIndex);
		urlString += encodeNumber(2, document.personal.hp_percent.value);

		var summon_type = document.getElementsByName("summon_type");
		var summon_percent = document.getElementsByName("summon_percent");

		for (var i=0; i<4; i++) {
			var summon_code = 0;
			var summon_num = 0;
			summon_code = arraySummon.indexOf(summon_type[i].value);

			if (eval(summon_percent[0].value) == null) {
				summon_num = 0;
			} else {
				summon_num = eval(summon_percent[i].value);
			}
			
			urlString += encodeNumber(1, summon_code);
			urlString += encodeNumber(2, summon_num);
		}
		
		var weapon_atk = document.getElementsByName("weapon_atk")
		var type_bonus = document.getElementsByName("type_bonus")
		var skill_type1 = document.getElementsByName("skill_type1")
		var skill_type2 = document.getElementsByName("skill_type2")
		var skill_lv = document.getElementsByName("skill_lv")

		for (var i=0; i<10; i++) {
			urlString += encodeNumber(3, eval(weapon_atk[i].value));
			var type_num = arrayType.indexOf(type_bonus[i].value);
			var skill_num1 = arraySkill.indexOf(skill_type1[i].value);
			var skill_num2 = arraySkill.indexOf(skill_type2[i].value);
			var level = skill_lv[i].selectedIndex;			
			urlString += encodeNumber(1, type_num);
			urlString += encodeNumber(1, skill_num1);
			urlString += encodeNumber(1, skill_num2);
			urlString += encodeNumber(1, level);
		}

		var summon_atk = document.getElementsByName("summon_atk")
		for (var i=0; i<5; i++) {
			urlString += encodeNumber(3, eval(summon_atk[i].value));
		}
		
		var buff_koujin    = document.personal.buff_koujin.value;
		var buff_attribute = document.personal.buff_attribute.value;
		var buff_multiply  = document.personal.buff_multiply.value;

		urlString += encodeNumber(2, eval(buff_koujin));
		urlString += encodeNumber(2, eval(buff_attribute));
		urlString += encodeNumber(2, eval(buff_multiply));
		
		var cosmos = document.getElementsByName("cosmos")
		for (var i=0; i<10; i++) {
			var flag = 0;
			if (cosmos[i].checked) flag =1;
			urlString += encodeNumber(1, flag);
		}

		return urlString;
	}

	function importUrl(urlString) {
		localUrl = urlString;
		if (urlString == undefined) localUrl = "";

		var flag = decodeNumber(2);

		if (flag < 1000) {
			/* 初期バージョン */
			document.personal.rank.value = flag;
			document.personal.job_atk.value = decodeNumber(3);
			document.personal.master_atk.value = decodeNumber(2);
			document.personal.ship_bonus.value = decodeNumber(2);
			document.personal.attribute_bonus.selectedIndex = decodeNumber(2);
			document.personal.hp_percent.value = decodeNumber(2);

			var summon_type = document.getElementsByName("summon_type");
			var summon_percent = document.getElementsByName("summon_percent");

			for (var i=0; i<2; i++) {
				summon_type[i*2].selectedIndex = decodeNumber(1);
				summon_percent[i*2].value = decodeNumber(2);
				summon_type[i*2+1].selectedIndex = 0;
				summon_percent[i*2+1].value = 0;
			}
			
			var weapon_atk = document.getElementsByName("weapon_atk")
			var type_bonus = document.getElementsByName("type_bonus")
			var skill_type1 = document.getElementsByName("skill_type1")
			var skill_lv = document.getElementsByName("skill_lv")

			for (var i=0; i<10; i++) {
				weapon_atk[i].value = decodeNumber(3);
				type_bonus[i].selectedIndex = decodeNumber(1);
				skill_type1[i].selectedIndex = decodeNumber(1);
				skill_lv[i].selectedIndex = decodeNumber(1);
			}

			document.personal.buff_koujin.value = 0;
			document.personal.buff_attribute.value = 0;
			document.personal.buff_multiply.value = 0;

			var summon_atk = document.getElementsByName("summon_atk")
			for (var i=0; i<5; i++) {
				summon_atk[i].value = decodeNumber(3);
			}

		} else {
			/* バージョンあり */
			var version = decodeNumber(2);
			if (version == 0) {
				document.personal.rank.value = decodeNumber(2);
				document.personal.job_atk.value = decodeNumber(3);
				document.personal.master_atk.value = decodeNumber(2);
				document.personal.ship_bonus.value = decodeNumber(2);
				document.personal.attribute_bonus.selectedIndex = decodeNumber(2);
				document.personal.hp_percent.value = decodeNumber(2);

				var summon_type = document.getElementsByName("summon_type");
				var summon_percent = document.getElementsByName("summon_percent");

				for (var i=0; i<2; i++) {
					summon_type[i*2].selectedIndex = decodeNumber(1);
					summon_percent[i*2].value = decodeNumber(2);
					summon_type[i*2+1].selectedIndex = 0;
					summon_percent[i*2+1].value = 0;
				}
				
				var weapon_atk = document.getElementsByName("weapon_atk")
				var type_bonus = document.getElementsByName("type_bonus")
				
				var skill_type1 = document.getElementsByName("skill_type1")
				var skill_type2 = document.getElementsByName("skill_type2")
				var skill_lv = document.getElementsByName("skill_lv")

				for (var i=0; i<10; i++) {
					weapon_atk[i].value = decodeNumber(3);
					type_bonus[i].selectedIndex = decodeNumber(1);
					skill_index1 = decodeNumber(1);
					skill_index2 = decodeNumber(1);
					var skill_value1 = arraySkill[skill_index1];
					var skill_html1 = arraySkillHTML.indexOf(skill_value1);
					var skill_value2 = arraySkill[skill_index2];
					var skill_html2 = arraySkillHTML.indexOf(skill_value2);
					skill_type1[i].selectedIndex = skill_html1;
					skill_type2[i].selectedIndex = skill_html2;
					skill_lv[i].selectedIndex = decodeNumber(1);
				}

				var summon_atk = document.getElementsByName("summon_atk")
				for (var i=0; i<5; i++) {
					summon_atk[i].value = decodeNumber(3);
				}

				document.personal.buff_koujin.value = 0;
				document.personal.buff_attribute.value = 0;
				document.personal.buff_multiply.value = 0;

				var cosmos = document.getElementsByName("cosmos")
				for (var i=0; i<10; i++) {
					cosmos[i].checked = false;
				}
			} if (version == 1 || version == 2) {
				document.personal.rank.value = decodeNumber(2);
				document.personal.job_atk.value = decodeNumber(3);
				document.personal.master_atk.value = decodeNumber(2);
				document.personal.ship_bonus.value = decodeNumber(2);
				document.personal.attribute_bonus.selectedIndex = decodeNumber(2);
				document.personal.hp_percent.value = decodeNumber(2);

				var summon_type = document.getElementsByName("summon_type");
				var summon_percent = document.getElementsByName("summon_percent");

				for (var i=0; i<4; i++) {
					summon_type[i].selectedIndex = decodeNumber(1);
					summon_percent[i].value = decodeNumber(2);
				}
				
				var weapon_atk = document.getElementsByName("weapon_atk")
				var type_bonus = document.getElementsByName("type_bonus")
				
				var skill_type1 = document.getElementsByName("skill_type1")
				var skill_type2 = document.getElementsByName("skill_type2")
				var skill_lv = document.getElementsByName("skill_lv")

				for (var i=0; i<10; i++) {
					weapon_atk[i].value = decodeNumber(3);
					type_bonus[i].selectedIndex = decodeNumber(1);
					skill_index1 = decodeNumber(1);
					skill_index2 = decodeNumber(1);
					var skill_value1 = arraySkill[skill_index1];
					var skill_html1 = arraySkillHTML.indexOf(skill_value1);
					var skill_value2 = arraySkill[skill_index2];
					var skill_html2 = arraySkillHTML.indexOf(skill_value2);
					skill_type1[i].selectedIndex = skill_html1;
					skill_type2[i].selectedIndex = skill_html2;
					skill_lv[i].selectedIndex = decodeNumber(1);
				}

				var summon_atk = document.getElementsByName("summon_atk")
				for (var i=0; i<5; i++) {
					summon_atk[i].value = decodeNumber(3);
				}

				document.personal.buff_koujin.value = decodeNumber(2);
				document.personal.buff_attribute.value = decodeNumber(2);
				document.personal.buff_multiply.value = decodeNumber(2);

				if (version == 2) {
					var cosmos = document.getElementsByName("cosmos")
					for (var i=0; i<10; i++) {
						if (decodeNumber(1) == 0) {
							cosmos[i].checked = false;
						} else {
							cosmos[i].checked = true;
						}
					}
				} else {
					for (var i=0; i<10; i++) {
						cosmos[i].checked = false;
					}
				}
			}
		}

		urlString = "http://hibin0.web.fc2.com/grbr_atk_calc/atk_calc.html?" + urlString;
		document.getElementById("url").value = urlString;
		
		update();
	}

	function numOnly(event) {
	  return true;
	  m = String.fromCharCode(event.keyCode);
	  if ( event.keyCode >= 96 && event.keyCode <= 105 ) return true;
	  if("0123456789¥t¥b¥r".indexOf(m, 0) < 0) return false;
	  return true;
	}

	function save_data(event) {
		var slot_num = document.system.save_slot.value;
		var party_name = document.system.patry_name.value;
		var slot_name = slot_num + "_name";
		var string = createUrl(string);
		var saved_data = localStorage.getItem("save_data")
		var save = saved_data === null ? {} : JSON.parse(saved_data)
		save[slot_num] = string;
		save[slot_name] = party_name;
		localStorage.setItem("save_data", JSON.stringify(save));
		
		slot_update();
	}

	function load_data(event) {
		var slot_num = document.system.save_slot.value;
		var slot_name = slot_num + "_name";
		var saved_data = localStorage.getItem("save_data")
		var save = saved_data === null ? {} : JSON.parse(saved_data)
		importUrl(save[slot_num]);
		if (save[slot_name] === undefined) {
			document.system.patry_name.value = "";
		} else {
			document.system.patry_name.value = save[slot_name];
		}
	}

	function getCookie(name)
	{
		var start = "";
		var end = "";

		if (document.cookie.length > 0) {
			start = document.cookie.indexOf(name + "=");
			if (start != -1) {
				start = start + name.length + 1;
				end = document.cookie.indexOf(";", start);
				if( end == -1 ) end = document.cookie.length;
				return decodeURIComponent( document.cookie.substring( start, end ) );
			}
		}
		
		return "";
	}

