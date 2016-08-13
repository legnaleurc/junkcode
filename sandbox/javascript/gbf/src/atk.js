/**
 * Created by romulus on 2016/1/21.
 */
const SLOT_NUM = 20;

const SKILL_NONE = 0;
const SKILL_ATK_SMALL = 1;
const SKILL_ATK_MID = 2;
const SKILL_ATK_LARGE = 3;
const SKILL_ATK_2 = 4;
const SKILL_SQUARE_ATK = 5;
const SKILL_SQUARE_ATK_2 = 6;
const SKILL_PINCH_SMALL = 7;
const SKILL_PINCH_MID = 8;
const SKILL_PINCH_LARGE = 9;
const SKILL_SQUARE_PINCH_SMALL = 10;
const SKILL_SQUARE_PINCH_MID = 11;
const SKILL_SQUARE_PINCH_LARGE = 12;
const SKILL_BAHAMUT_ATK = 13;
const SKILL_BAHAMUT_ATK_HP = 14;
const SKILL_UNKNOWN_1 = 15;
const SKILL_UNKNOWN_2 = 16;
const SKILL_OTHER_ATK_MEDIUM = 21;
const SKILL_OTHER_ATK = 17;
const SKILL_SQUARE_KAMUI = 18;
const SKILL_KAMUI = 19;
const SKILL_COSMOS = 20;

const BUFF_NONE = 0;
const BUFF_ATK_REGULAR = 1;
const BUFF_ATK_ATTR = 2;
const BUFF_ATK_OTHER = 3;
const DEBUFF_DEF = 4;
const CRITICAL = 5;
const DEBUFF_ATK = 6;
const BUFF_DEF = 7;
const DAMAGE_CUT = 8;
const ABILITY_RATE = 9;
const ENEMY_DEF = 10;

$().ready(function () {
    Parse.initialize("LezNaQSX58vqVLetBTLPzxLv1SasY2Jy3joVkGaC", "9pmVcL7Ee1zPmPVM3zmCRsapu2kTx6veBTrmom4Q");
    initInfo();
    initWeaponWarehouse();
    loadCookies();
    $('#cloud_name').on('change', saveCookies);
    for (var slot_num = 0; slot_num < SLOT_NUM; slot_num++) {
        $('#save_slot option').eq(slot_num).html($.sprintf('SLOT%d %s', slot_num + 1,
            localStorage.getItem('slot_' + slot_num + '_name')));
    }
    update();
    $('#info input,select').on('change', update);
});

function initInfo() {
    $('#info input,select').each(function (index, ele) {
        var $ele = $(ele);
        var id = $ele.attr('id');
        if (id) {
            $ele.attr('name', id)
        } else {
            var class_name = $ele.attr('class')
            $ele.attr('name', class_name + "_" + $ele.index('.' + class_name))
        }
    });
}

function initWeaponWarehouse() {
    var $ware_list = $('#ware_weapon_list');
    $ware_list.empty();
    for (var i = 0; i < 15; i++) {
        var weapon = new Weapon();
        $ware_list.append(weapon.tojQObj());
    }
}

function equip(ele) {
    var $ele = $(ele).parent().parent();
    var $ware_list = $('#ware_weapon_list');
    var $equipped_list = $('#equipped_weapon_list');
    if ($ele.hasClass('weapon')) {
        if ($ware_list.find($ele).length > 0) {
            if ($equipped_list.children('.weapon').length < 10) {
                $ele.detach();
                $equipped_list.append($ele);
            }
        } else {
            $ele.detach();
            $ware_list.append($ele);
        }
        $('#equipped_count').html($equipped_list.children('.weapon').length);
    } else if ($ele.hasClass('buff')) {
        if ($ware_list.find($ele).length > 0) {
            $ele.detach();
            $equipped_list.append($ele);
        } else {
            $ele.detach();
            $ware_list.append($ele);
        }
    }
    update();
}

function update() {
    // 基本攻撃力
    var basic_atk = 0;

    var rank = parseInt($('#rank').val());
    if (rank) {
        if (rank == 1) {
            basic_atk += 1000;
        } else if (rank <= 100) {
            basic_atk += 1000 + 40 * rank;
        } else {
            basic_atk += 5000 + 20 * (rank - 100);
        }
    }

    var job_bonus = parseInt($('#job_bonus').val());
    if (job_bonus) basic_atk += job_bonus;

    $('.summon_atk').each(function (index, summon) {
        var s_atk = parseInt(summon.value);
        if (s_atk) {
            basic_atk += s_atk;
        }
    });

    var regular_atk = 0;
    var square_atk = 0;
    var unknown_atk = 0;
    var strength = 0;
    var regular_pinch = 0;
    var regular_pinch_lv = 0;
    var square_pinch = 0;
    var square_pinch_lv = 0;
    var bahamut = 0;

    var normal_buff = 0;
    var attribute_mod;

    var summon_regular = 0;
    var summon_magna = 0;
    var summon_unknown = 0;

    var cosmos_type = -1;
    var cosmos_effect = 0;

    var $equipped_list = $('#equipped_weapon_list');
    // コスモスチェック
    $equipped_list.children('tr.weapon').each(function (index, tr) {
        var $tr = $(tr);
        var weapon_atk = parseInt($tr.find('.atk').val());
        if (!weapon_atk) return;
        for (var i = 1; i <= 2; i++) {
            var $skill = $tr.find('.skill' + i);
            if (parseInt($skill.val()) == SKILL_COSMOS) {
                cosmos_type = $tr.find('.type').val();
            }
        }
    });

    $equipped_list.children('tr.weapon').each(function (index, tr) {
        var $tr = $(tr);
        var weapon_atk = parseInt($tr.find('.atk').val());
        if (!weapon_atk) return;

        // 得意武器
        var mw1 = $('.mastered_weapon').eq(0).val();
        var mw2 = $('.mastered_weapon').eq(1).val();
        var type = $tr.find('.type').val();
        var cosmos = 0;
        if (type == cosmos_type) {
            cosmos = 0.3;
        }
        if (mw1 == mw2) {
            if (mw1 == type || mw2 == type) {
                var org_atk = weapon_atk *
                    (1.2 + 0.01 * (parseInt($('.wlv').eq(0).val()) + parseInt($('.wlv').eq(1).val())));
                weapon_atk *= 1.2 + cosmos + 0.01 * (parseInt($('.wlv').eq(0).val()) + parseInt($('.wlv').eq(1).val()));
                cosmos_effect += weapon_atk - org_atk;
            }
        } else {
            var org_atk;
            if (mw1 == type) {
                org_atk = weapon_atk * (1.2 + 0.01 * (parseInt($('.wlv').eq(0).val())));
                weapon_atk *= 1.2 + cosmos + 0.01 * (parseInt($('.wlv').eq(0).val()));
            } else if (mw2 == type) {
                org_atk = weapon_atk * (1.2 + 0.01 * (parseInt($('.wlv').eq(1).val())));
                weapon_atk *= 1.2 + cosmos + 0.01 * (parseInt($('.wlv').eq(1).val()));
            } else {
                org_atk = weapon_atk;
                weapon_atk *= 1 + cosmos;
            }
            cosmos_effect += weapon_atk - org_atk;
        }
        basic_atk += weapon_atk;

        // スキル
        var slv = parseInt($tr.find('.slv').val());
        if (!slv) return;
        var damage = 100 - parseInt($('#hp_left').val());
        if (!damage) damage = 0;
        for (var i = 1; i <= 2; i++) {
            var $skill = $tr.find('.skill' + i);
            switch (parseInt($skill.val())) {
                case SKILL_ATK_SMALL:
                    if (slv <= 10) {
                        regular_atk += slv;
                    } else {
                        regular_atk += 10 + 0.4 * (slv - 10);
                    }
                    break;
                case SKILL_ATK_MID:
                    if (slv <= 10) {
                        regular_atk += 2 + slv;
                    } else {
                        regular_atk += 12 + 0.5 * (slv - 10);
                    }
                    break;
                case SKILL_ATK_LARGE:
                    if (slv <= 10) {
                        regular_atk += 5 + slv;
                    } else {
                        regular_atk += 15 + 0.6 * (slv - 10);
                    }
                    break;
                case SKILL_ATK_2:
                    if (slv <= 10) {
                        regular_atk += 6 + slv;
                    } else {
                        regular_atk += 16 + 0.8 * (slv - 10);
                    }
                    break;
                case SKILL_SQUARE_ATK:
                    if (slv <= 10) {
                        square_atk += 2 + slv;
                    } else {
                        square_atk += 12 + 0.6 * (slv - 10);
                    }
                    break;
                case SKILL_SQUARE_ATK_2:
                    if (slv <= 10) {
                        square_atk += 5 + slv;
                    } else {
                        square_atk += 15 + 0.6 * (slv - 10);
                    }
                    break;
                case SKILL_SQUARE_KAMUI:
                    if (slv <= 10) {
                        square_atk += slv;
                    } else {
                        square_atk += 10 + 0.6 * (slv - 10);
                    }
                    break;
                case SKILL_KAMUI:
                    if (slv <= 10) {
                        regular_atk += slv;
                    } else {
                        regular_atk += 10 + 0.6 * (slv - 10);
                    }
                    break;
                case SKILL_PINCH_SMALL:
                    regular_pinch += (0.04 + 0.002 * slv) * damage * (1 + 0.02 * damage);
                    regular_pinch_lv += 20 + slv;
                    break;
                case SKILL_PINCH_MID:
                    regular_pinch += (0.06 + 0.002 * slv) * damage * (1 + 0.02 * damage);
                    regular_pinch_lv += 30 + slv;
                    break;
                case SKILL_PINCH_LARGE:
                    regular_pinch += (0.08 + 0.002 * slv) * damage * (1 + 0.02 * damage);
                    regular_pinch_lv += 40 + slv;
                    break;
                case SKILL_SQUARE_PINCH_SMALL:
                    square_pinch += (0.04 + 0.002 * slv) * damage * (1 + 0.02 * damage);
                    square_pinch_lv += 20 + slv;
                    break;
                case SKILL_SQUARE_PINCH_MID:
                    square_pinch += (0.06 + 0.002 * slv) * damage * (1 + 0.02 * damage);
                    square_pinch_lv += 30 + slv;
                    break;
                case SKILL_SQUARE_PINCH_LARGE:
                    square_pinch += (0.08 + 0.002 * slv) * damage * (1 + 0.02 * damage);
                    square_pinch_lv += 40 + slv;
                    break;
                case SKILL_BAHAMUT_ATK:
                    normal_buff += 19 + slv;
                    bahamut += 19 + slv;
                    if (slv == 10) {
                        normal_buff += 1;
                        bahamut += 1;
                    }
                    break;
                case SKILL_BAHAMUT_ATK_HP:
                    normal_buff += 9.5 + 0.5 * slv;
                    bahamut += 9.5 + 0.5 * slv;
                    if (slv == 10) {
                        normal_buff += 0.5;
                        bahamut += 0.5;
                    }
                    break;
                case SKILL_UNKNOWN_1:
                    if (slv <= 10) {
                        unknown_atk += 2 + slv;
                    } else {
                        unknown_atk += 12 + 0.5 * (slv - 10);
                    }
                    break;
                case SKILL_UNKNOWN_2:
                    if (slv <= 10) {
                        unknown_atk += 5 + slv;
                    } else {
                        unknown_atk += 15 + 0.6 * (slv - 10);
                    }
                    break;
                case SKILL_OTHER_ATK:
                    if (slv <= 10) {
                        strength += 5 + slv;
                    } else {
                        strength += 15 + 0.6 * (slv - 10);
                    }
                    break;
            }
        }
    });

    // バハ武器上限
    console.log(bahamut)
    if (bahamut > 50) {
        normal_buff -= bahamut - 50;
    }

    // 属性
    attribute_mod = parseInt($('#attribute_bonus').val());

    var enemy_def = 10;
    var other_buff = [];
    var def_buff = 0;
    var def_debuff = 0;
    var atk_debuff = 0;
    var damage_cut = 100;
    var critical = 0;
    var ability_rate = 1;

    $equipped_list.children('tr.buff').each(function (index, tr) {
        var $tr = $(tr);
        var value = parseFloat($tr.find('.value').val());
        if (!value) return;

        var $type = $tr.find('.type');
        switch (parseInt($type.val())) {
            case BUFF_ATK_REGULAR:
                normal_buff += value;
                break;
            case BUFF_ATK_ATTR:
                attribute_mod += value;
                break;
            case BUFF_ATK_OTHER:
                other_buff.push(value);
                break;
            case ABILITY_RATE:
                ability_rate = value;
                break;
            case ENEMY_DEF:
                enemy_def = value;
                break;
            case CRITICAL:
                critical += value;
                break;
            case BUFF_DEF:
                def_buff += value;
                break;
            case DEBUFF_ATK:
                atk_debuff += value;
                break;
            case DEBUFF_DEF:
                def_debuff += value;
                break;
            case DAMAGE_CUT:
                damage_cut -= value;
                break;
        }
    });

    // 加護
    for (var i = 0; i < 2; i++) {
        var summon_type = $('.summon_type').eq(i).val();
        var summon_value = parseInt($('.summon_value').eq(i).val());
        if (!summon_value) continue;
        switch (summon_type) {
            case 'attribute':
                attribute_mod += summon_value;
                break;
            case 'character':
                normal_buff += summon_value;
                break;
            case 'magna':
                summon_magna += summon_value;
                break;
            case 'unknown':
                summon_unknown += summon_value;
                break;
            case 'zeus':
                summon_regular += summon_value;
                break;
        }
    }

    var master_bonus = parseInt($('#master_bonus').val());
    basic_atk = Math.floor(basic_atk);
    if (master_bonus) basic_atk *= (1 + 0.01 * master_bonus);

    $('#basic_atk').html(Math.round(basic_atk));
    $('#attribute_mod').html(attribute_mod + '％');

    $('#regular_atk').html(Math.round(regular_atk * (1 + 0.01 * summon_regular)) + '％');
    $('#square_atk').html(Math.round(square_atk * (1 + 0.01 * summon_magna)) + '％');
    $('#strength').html(strength + '％');
    $('#unknown_atk').html(Math.round(unknown_atk * (1 + 0.01 * summon_unknown)) + '％');
    $('#normal_buff').html(normal_buff + '％');
    $('#regular_pinch').html($.sprintf('%.2f％', Math.round(regular_pinch * (1 + 0.01 * summon_regular))));
    $('#square_pinch').html($.sprintf('%.2f％', Math.round(square_pinch * (1 + 0.01 * summon_magna))));

    // スキル修正適用
    var skilled_atk = basic_atk;
    var square_atk_clause = 1 + 0.01 * square_atk * (1 + 0.01 * summon_magna);
    skilled_atk *= square_atk_clause;
    var square_pinch_clause = 1 + 0.01 * square_pinch * (1 + 0.01 * summon_magna);
    skilled_atk *= square_pinch_clause;
    var unknown_clause = 1 + 0.01 * unknown_atk * (1 + 0.01 * summon_unknown) + 0.01 * strength;
    skilled_atk *= unknown_clause;
    var regular_atk_clause = 1 + 0.01 * regular_atk * (1 + 0.01 * summon_regular) + 0.01 * normal_buff;
    skilled_atk *= regular_atk_clause;
    var regular_pinch_clause = 1 + 0.01 * regular_pinch * (1 + 0.01 * summon_regular);
    skilled_atk *= regular_pinch_clause;
    skilled_atk *= 0.01 * attribute_mod;

    // 最終修正
    var final_atk = Math.floor(skilled_atk * (1 - 0.01 * atk_debuff) * (1 + 0.01 * critical) * ability_rate);

    // 騎空艇
    var ship_bonus = parseInt($('#ship_bonus').val());
    if (ship_bonus) final_atk *= 1 + 0.01 * ship_bonus;

    // 別枠バフ
    for (var bi in other_buff) {
        final_atk *= (1 + 0.01 * other_buff[bi]);
    }

    // 前との攻撃力変化を算出
    if (basic_atk > 0) {
        $('#magnification').html($.sprintf('%.2f', final_atk / basic_atk));
    }
    final_atk = Math.round(final_atk);
    var prev_final = parseInt($('.final_atk_val').eq(0).html());
    if (prev_final) {
        var $diff = $('#diff');
        if (final_atk > prev_final) {
            $diff.html('<span style="color: green;">(+' + (final_atk - prev_final) + ')</span>');
        } else {
            $diff.html('<span style="color: red;">(' + (final_atk - prev_final) + ')</span>');
        }
        $diff.show();
        setTimeout(function () {
            $diff.fadeOut();
        }, 1000);
    }
    $('.final_atk_val').html(final_atk);

    // ダメージ算出
    enemy_def *= 1 + 0.01 * Math.max(-50, def_buff - def_debuff);
    $('#enemy_def').html($.sprintf('%.1f', enemy_def));

    var damage = final_atk * (damage_cut / 100.0) / enemy_def;
    var prev_damage = parseInt($('.damage_center_val').eq(0).html());
    if (prev_damage) {
        var $dmg_diff = $('#dmg_diff');
        if (Math.ceil(damage) > prev_damage) {
            $dmg_diff.html('<span style="color: green;">(+' + (Math.ceil(damage) - prev_damage) + ')</span>');
        } else {
            $dmg_diff.html('<span style="color: red;">(' + (Math.ceil(damage) - prev_damage) + ')</span>');
        }
        $dmg_diff.show();
        setTimeout(function () {
            $dmg_diff.fadeOut();
        }, 1000);
    }

    var damage_list = '';
    for (var s = 950; s <= 1050; s += 1) {
        damage_list += Math.ceil(damage * s / 1000.0) + ', ';
        if (s % 10 == 9) {
            damage_list += '\n';
        }
    }
    damage_list = damage_list.slice(0, -2);
    $('.damage_val').html($.sprintf('%d～%d', Math.ceil(damage * 0.95), Math.ceil(damage * 1.05)))
        .attr('title', damage_list);
    $('.damage_center_val').html($.sprintf('%d', Math.ceil(damage)));

    // 各強化パートの１％増し効果
    $('#square_atk_per').html(
        $.sprintf('+%d', final_atk / square_atk_clause * (1 + 0.01 * (square_atk + 1) * (1 + 0.01 * summon_magna)) - final_atk)
    );
    $('#unknown_atk_per').html(
        $.sprintf('+%d', final_atk / unknown_clause * (1 + 0.01 * (unknown_atk + 1) * (1 + 0.01 * summon_unknown) + 0.01 * strength) - final_atk)
    );
    $('#strength_per').html(
        $.sprintf('+%d', final_atk / unknown_clause * (1 + 0.01 * unknown_atk * (1 + 0.01 * summon_unknown) + 0.01 * (strength + 1)) - final_atk)
    );
    $('#regular_atk_per').html(
        $.sprintf('+%d', final_atk / regular_atk_clause * (1 + 0.01 * (regular_atk + 1) * (1 + 0.01 * summon_regular) + 0.01 * normal_buff) - final_atk)
    );
    $('#normal_buff_per').html(
        $.sprintf('+%d', final_atk / regular_atk_clause * (1 + 0.01 * regular_atk * (1 + 0.01 * summon_regular) + 0.01 * (normal_buff + 1)) - final_atk)
    );

    // 背水レベル
    $('#regular_pinch_lv').html($.sprintf('(%d)', regular_pinch_lv));
    $('#square_pinch_lv').html($.sprintf('(%d)', square_pinch_lv));

    // 武器の実質攻撃力算出
    for (var jj = 0; jj < 2; jj++) {
        [$equipped_list, $('#ware_weapon_list')][jj].children('tr.weapon').each(function (index, tr) {
            var real_atk = 0;

            var $tr = $(tr);
            var weapon_atk = parseInt($tr.find('.atk').val());
            if (!weapon_atk) return;

            // 得意武器
            var mw1 = $('.mastered_weapon').eq(0).val();
            var mw2 = $('.mastered_weapon').eq(1).val();
            var type = $tr.find('.type').val();
            if (mw1 == mw2) {
                if (mw1 == type || mw2 == type) {
                    weapon_atk *= 1.2 + 0.01 * (parseInt($('.wlv').eq(0).val()) + parseInt($('.wlv').eq(1).val()));
                }
            } else {
                if (mw1 == type) {
                    weapon_atk *= 1.2 + 0.01 * (parseInt($('.wlv').eq(0).val()));
                } else if (mw2 == type) {
                    weapon_atk *= 1.2 + 0.01 * (parseInt($('.wlv').eq(1).val()));
                }
            }

            // 基本攻撃力分
            real_atk += final_atk * weapon_atk / basic_atk;

            // スキル
            var slv = parseInt($tr.find('.slv').val());
            if (!slv && slv != 0) return;
            var damage = 100 - parseInt($('#hp_left').val());
            if (!damage) damage = 0;
            for (var i = 1; i <= 2; i++) {
                var $skill = $tr.find('.skill' + i);
                var val;
                switch (parseInt($skill.val())) {
                    case SKILL_ATK_SMALL:
                        if (slv <= 10) {
                            val = slv;
                        } else {
                            val = 10 + 0.4 * (slv - 10);
                        }
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_regular) / regular_atk_clause;
                        break;
                    case SKILL_ATK_MID:
                        if (slv <= 10) {
                            val = 2 + slv;
                        } else {
                            val = 12 + 0.5 * (slv - 10);
                        }
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_regular) / regular_atk_clause;
                        break;
                    case SKILL_ATK_LARGE:
                        if (slv <= 10) {
                            val = 5 + slv;
                        } else {
                            val = 15 + 0.6 * (slv - 10);
                        }
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_regular) / regular_atk_clause;
                        break;
                    case SKILL_ATK_2:
                        if (slv <= 10) {
                            val = 6 + slv;
                        } else {
                            val = 16 + 0.8 * (slv - 10);
                        }
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_regular) / regular_atk_clause;
                        break;
                    case SKILL_SQUARE_ATK:
                        if (slv <= 10) {
                            val = 2 + slv;
                        } else {
                            val = 12 + 0.6 * (slv - 10);
                        }
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_magna) / square_atk_clause;
                        break;
                    case SKILL_SQUARE_ATK_2:
                        if (slv <= 10) {
                            val = 5 + slv;
                        } else {
                            val = 15 + 0.6 * (slv - 10);
                        }
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_magna) / square_atk_clause;
                        break;
                    case SKILL_SQUARE_KAMUI:
                        if (slv <= 10) {
                            val = slv;
                        } else {
                            val = 10 + 0.6 * (slv - 10);
                        }
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_magna) / square_atk_clause;
                        break;
                    case SKILL_KAMUI:
                        if (slv <= 10) {
                            val = slv;
                        } else {
                            val = 10 + 0.6 * (slv - 10);
                        }
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_regular) / regular_atk_clause;
                        break;
                    case SKILL_PINCH_SMALL:
                        val = (0.04 + 0.002 * slv) * damage * (1 + 0.02 * damage);
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_regular) / regular_pinch_clause;
                        break;
                    case SKILL_PINCH_MID:
                        val = (0.06 + 0.002 * slv) * damage * (1 + 0.02 * damage);
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_regular) / regular_pinch_clause;
                        break;
                    case SKILL_PINCH_LARGE:
                        val = (0.08 + 0.002 * slv) * damage * (1 + 0.02 * damage);
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_regular) / regular_pinch_clause;
                        break;
                    case SKILL_SQUARE_PINCH_SMALL:
                        val = (0.04 + 0.002 * slv) * damage * (1 + 0.02 * damage);
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_magna) / square_pinch_clause;
                        break;
                    case SKILL_SQUARE_PINCH_MID:
                        val = (0.06 + 0.002 * slv) * damage * (1 + 0.02 * damage);
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_magna) / square_pinch_clause;
                        break;
                    case SKILL_SQUARE_PINCH_LARGE:
                        val = (0.08 + 0.002 * slv) * damage * (1 + 0.02 * damage);
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_magna) / square_pinch_clause;
                        break;
                    case SKILL_BAHAMUT_ATK:
                        if (slv > 0) {
                            val = 19 + slv;
                            if (slv == 10) {
                                val = 30;
                            }
                            real_atk += final_atk * (bahamut > 50 ? 50.0 / bahamut : 1) * val * 0.01 / regular_atk_clause;
                        }
                        break;
                    case SKILL_BAHAMUT_ATK_HP:
                        if (slv > 0) {
                            val = 9.5 + 0.5 * slv;
                            if (slv == 10) {
                                val = 15;
                            }
                            real_atk += final_atk * (bahamut > 50 ? 50.0 / bahamut : 1) * val * 0.01 / regular_atk_clause;
                        }
                        break;
                    case SKILL_UNKNOWN_1:
                        if (slv <= 10) {
                            val = 2 + slv;
                        } else {
                            val = 12 + 0.5 * (slv - 10);
                        }
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_unknown) / unknown_clause;
                        break;
                    case SKILL_UNKNOWN_2:
                        if (slv <= 10) {
                            val = 5 + slv;
                        } else {
                            val = 15 + 0.6 * (slv - 10);
                        }
                        real_atk += final_atk * val * 0.01 * (1 + 0.01 * summon_unknown) / unknown_clause;
                        break;
                    case SKILL_OTHER_ATK_MEDIUM:
                        if (slv <= 10) {
                            val = 2 + slv;
                        } else {
                            val = 12 + 0.5 * (slv - 10);
                        }
                        real_atk += final_atk * val * 0.01 / unknown_clause;
                        break;
                    case SKILL_OTHER_ATK:
                        if (slv <= 10) {
                            val = 5 + slv;
                        } else {
                            val = 15 + 0.6 * (slv - 10);
                        }
                        real_atk += final_atk * val * 0.01 / unknown_clause;
                        break;
                    case SKILL_COSMOS:
                        real_atk = final_atk * (cosmos_effect / real_atk);
                        break;
                }
            }
            $tr.find('.real_atk').html(Math.round(real_atk));
        });
    }

    // バフの実質攻撃力算出
    for (var kk = 0; kk < 2; kk++) {
        [$equipped_list, $('#ware_weapon_list')][kk].children('tr.buff').each(function (index, tr) {
            var real_damage = 0;

            var $tr = $(tr);
            var value = parseFloat($tr.find('.value').val());

            var $type = $tr.find('.type');
            switch (parseInt($type.val())) {
                case BUFF_ATK_REGULAR:
                    real_damage = damage * value * 0.01 / regular_atk_clause;
                    break;
                case BUFF_ATK_ATTR:
                    real_damage = damage * value / attribute_mod;
                    break;
                case BUFF_ATK_OTHER:
                    var obttl = 0;
                    for (var ob in other_buff) {
                        obttl += other_buff[ob];
                    }
                    real_damage = damage * value / (100 + obttl);
                    break;
                case ABILITY_RATE:
                    if ($tr.parent().parent().parent().attr('id') == 'equipped') {
                        real_damage = 0;
                    } else {
                        real_damage = damage * value;
                    }
                    break;
                case ENEMY_DEF:
                    real_damage = 0;
                    break;
                case CRITICAL:
                    real_damage = damage * value / (100 + critical);
                    break;
                case BUFF_DEF:
                    var org_damage = damage * (0.01 * (100 + def_buff - def_debuff));
                    if ($tr.parent().parent().parent().attr('id') == 'equipped') {
                        var add_damage = org_damage * (def_debuff - def_buff) / (100 + def_buff - def_debuff);
                        real_damage = add_damage * value / def_buff;
                    } else {
                        real_damage = org_damage / (0.01 * (100 + def_buff - def_debuff + value)) - damage;
                    }
                    break;
                case DEBUFF_ATK:
                    if ($tr.parent().parent().parent().attr('id') == 'equipped') {
                        real_damage = -(damage * (1 / (0.01 * (100 - atk_debuff)) - 1)) * (value / atk_debuff);
                    } else {
                        real_damage = -damage * value / (100 + atk_debuff);
                    }
                    break;
                case DEBUFF_DEF:
                    var org_damage = damage * (0.01 * (100 + def_buff - def_debuff));
                    if ($tr.parent().parent().parent().attr('id') == 'equipped') {
                        var add_damage = org_damage * (def_debuff - def_buff) / (100 + def_buff - def_debuff);
                        real_damage = add_damage * value / def_debuff;
                    } else {
                        real_damage = org_damage / (0.01 * (100 + def_buff - def_debuff - value)) - damage;
                    }
                    break;
                case DAMAGE_CUT:
                    real_damage = damage - (damage / (1 - 0.01 * value));
                    break;
                case BUFF_NONE:
                    real_damage = 0;
                    break;
            }

            if (real_damage != 0) {
                $tr.find('.real_damage').html(Math.round(real_damage));
            } else {
                $tr.find('.real_damage').html('-');
            }
        });

    }
}

function save_data() {
    var slot_num = parseInt($('#save_slot').val());
    var slot_name = $('#slot_name').val();
    localStorage.setItem('slot_' + slot_num + '_name', slot_name);
    localStorage.setItem('slot_' + slot_num + '_info', JSON.stringify($('#info').values()));

    var $equipped = $('#equipped_weapon_list .weapon');
    localStorage.setItem('slot_' + slot_num + '_equip_num', $equipped.length);
    $equipped.each(function (index, tr) {
        localStorage.setItem('slot_' + slot_num + '_equip_' + index, JSON.stringify(Weapon.toJSON($(tr))));
    });

    var $warehouse = $('#ware_weapon_list .weapon');
    localStorage.setItem('slot_' + slot_num + '_warehouse_num', $warehouse.length);
    $warehouse.each(function (index, tr) {
        localStorage.setItem('slot_' + slot_num + '_warehouse_' + index, JSON.stringify(Weapon.toJSON($(tr))));
    });

    var $equipped_buff = $('#equipped_weapon_list .buff');
    localStorage.setItem('slot_' + slot_num + '_buff_num', $equipped_buff.length);
    $equipped_buff.each(function (index, tr) {
        localStorage.setItem('slot_' + slot_num + '_buff_' + index, JSON.stringify(Buff.toJSON($(tr))));
    });

    var $warehouse_buff = $('#ware_weapon_list .buff');
    localStorage.setItem('slot_' + slot_num + '_warehouse_buff_num', $warehouse_buff.length);
    $warehouse_buff.each(function (index, tr) {
        localStorage.setItem('slot_' + slot_num + '_warehouse_buff_' + index, JSON.stringify(Buff.toJSON($(tr))));
    });

    $('#save_slot option').eq(slot_num).html($.sprintf('SLOT%d %s', slot_num + 1, slot_name));
    $('#save_message').show();
    setTimeout(function () {
        $('#save_message').fadeOut(1000)
    }, 1000);
}

function load_data() {
    var slot_num = $('#save_slot').val();
    $('#slot_name').val(localStorage.getItem('slot_' + slot_num + '_name'));
    var info = localStorage.getItem('slot_' + slot_num + '_info');
    if (info === null) return;
    $('#info').values(JSON.parse(info));

    var $equipped = $('#equipped_weapon_list');
    $equipped.empty();
    var eq_count = localStorage.getItem('slot_' + slot_num + '_equip_num');
    for (var e = 0; e < eq_count; e++) {
        var weapon = new Weapon(JSON.parse(localStorage.getItem('slot_' + slot_num + '_equip_' + e)));
        $equipped.append(weapon.tojQObj());
    }
    var buff_count = localStorage.getItem('slot_' + slot_num + '_buff_num');
    for (var be = 0; be < buff_count; be++) {
        var buff = new Buff(JSON.parse(localStorage.getItem('slot_' + slot_num + '_buff_' + be)));
        $equipped.append(buff.tojQObj());
    }

    var $warehouse = $('#ware_weapon_list');
    $warehouse.empty();
    var ware_count = localStorage.getItem('slot_' + slot_num + '_warehouse_num');
    for (var e = 0; e < ware_count; e++) {
        var weapon = new Weapon(JSON.parse(localStorage.getItem('slot_' + slot_num + '_warehouse_' + e)));
        $warehouse.append(weapon.tojQObj());
    }
    var ware_buff_count = localStorage.getItem('slot_' + slot_num + '_warehouse_buff_num');
    for (var wbe = 0; wbe < ware_buff_count; wbe++) {
        var buff = new Buff(JSON.parse(localStorage.getItem('slot_' + slot_num + '_warehouse_buff_' + wbe)));
        $warehouse.append(buff.tojQObj());
    }

    $('#equipped_count').html($equipped.children('.weapon').length);
    update();
    $('#load_message').show();
    setTimeout(function () {
        $('#load_message').fadeOut(1000)
    }, 1000);
}

function upload() {
    $('#uploading').show();
    var SlotGroup = Parse.Object.extend("SlotGroup");
    var query = new Parse.Query(SlotGroup);
    query.equalTo('owner', $('#cloud_name').val());
    query.find({
        success: function (results) {
            var object = results[0];
            saveToCloud(object);
        },
        error: function (error) {
            saveToCloud(null);
        }
    });
}

function saveToCloud(slot_group) {
    var SlotGroup = Parse.Object.extend("SlotGroup");
    if (!slot_group) {
        slot_group = new SlotGroup();
    }

    var data = [];
    for (var slot_num = 0; slot_num < SLOT_NUM; slot_num++) {
        var slot = {};
        slot.name = localStorage.getItem('slot_' + slot_num + '_name');
        slot.info = localStorage.getItem('slot_' + slot_num + '_info');
        slot.eq_count = localStorage.getItem('slot_' + slot_num + '_equip_num');
        slot.eq = [];
        for (var e = 0; e < slot.eq_count; e++) {
            slot.eq.push(localStorage.getItem('slot_' + slot_num + '_equip_' + e));
        }
        slot.ware_count = localStorage.getItem('slot_' + slot_num + '_warehouse_num');
        slot.ware = [];
        for (var e = 0; e < slot.ware_count; e++) {
            slot.ware.push(localStorage.getItem('slot_' + slot_num + '_warehouse_' + e));
        }
        slot.buff_count = localStorage.getItem('slot_' + slot_num + '_buff_num');
        slot.buff = [];
        for (var be = 0; be < slot.eq_count; be++) {
            slot.buff.push(localStorage.getItem('slot_' + slot_num + '_buff_' + be));
        }
        slot.ware_buff_count = localStorage.getItem('slot_' + slot_num + '_warehouse_buff_num');
        slot.ware_buff = [];
        for (var bwe = 0; bwe < slot.ware_count; bwe++) {
            slot.ware_buff.push(localStorage.getItem('slot_' + slot_num + '_warehouse_buff_' + bwe));
        }
        data.push(slot);
    }

    slot_group.set('owner', $('#cloud_name').val());
    slot_group.set('data', data);
    slot_group.save(null, {
        success: function (obj) {
            $('#uploading').hide();
            $('#upload_message').show();
            setTimeout(function () {
                $('#upload_message').fadeOut(1000)
            }, 1000);
        },
        error: function (deckObj, error) {
            alert(error.message);
            console.log(error);
            $('#uploading').hide();
        }
    });
}

function download() {
    $('#downloading').show();
    var SlotGroup = Parse.Object.extend("SlotGroup");
    var query = new Parse.Query(SlotGroup);
    query.equalTo('owner', $('#cloud_name').val());
    query.find({
        success: function (results) {
            var slots = results[0].get('data');

            for (var slot_num = 0; slot_num < SLOT_NUM; slot_num++) {
                var data = slots[slot_num];
                if (!data) continue;
                localStorage.setItem('slot_' + slot_num + '_name', data.name ? data.name : '');
                localStorage.setItem('slot_' + slot_num + '_info', data.info);

                var eq_count = parseInt(data.eq_count);
                localStorage.setItem('slot_' + slot_num + '_equip_num', eq_count);
                for (var e = 0; e < eq_count; e++) {
                    localStorage.setItem('slot_' + slot_num + '_equip_' + e, data.eq[e]);
                }

                var ware_count = parseInt(data.ware_count);
                localStorage.setItem('slot_' + slot_num + '_warehouse_num', ware_count);
                for (var index = 0; index < ware_count; index++) {
                    localStorage.setItem('slot_' + slot_num + '_warehouse_' + index, data.ware[index]);
                }

                var buff_count = parseInt(data.buff_count);
                localStorage.setItem('slot_' + slot_num + '_buff_num', buff_count);
                for (var be = 0; be < buff_count; be++) {
                    localStorage.setItem('slot_' + slot_num + '_buff_' + be, data.buff[be]);
                }

                var ware_buff_count = parseInt(data.ware_buff_count);
                localStorage.setItem('slot_' + slot_num + '_warehouse_buff_num', ware_buff_count);
                for (var bwf = 0; bwf < ware_buff_count; bwf++) {
                    localStorage.setItem('slot_' + slot_num + '_warehouse_buff_' + bwf, data.ware[bwf]);
                }

                $('#save_slot option').eq(slot_num).html($.sprintf('SLOT%d %s', slot_num + 1, data.name ? data.name : ''));
            }

            $('#save_slot').val('0');
            load_data();

            $('#downloading').hide();
            $('#download_message').show();
            setTimeout(function () {
                $('#download_message').fadeOut(1000)
            }, 1000);
        },
        error: function (deckObj, error) {
            alert(error.message);
            console.log(error);
            $('#downloading').hide();
        }
    });
}

function addWeapon() {
    var $ware_list = $('#ware_weapon_list');
    var weapon = new Weapon();
    $ware_list.append(weapon.tojQObj());
}

function addBuff() {
    var $eq_list = $('#equipped_weapon_list');
    var buff = new Buff();
    $eq_list.append(buff.tojQObj());
}

function deleteWeapon() {
    var $ware_list = $('#ware_weapon_list');
    $ware_list.children().eq(-1).remove();
}

function sortEquip() {
    var $equip_list = $('#equipped_weapon_list');
    var weapons = [];
    $equip_list.children().each(function (index, ele) {
        weapons.push($(ele).detach());
    });
    weapons.sort(function (a, b) {
        var va = parseInt(a.find('.real_atk').html());
        var vb = parseInt(b.find('.real_atk').html());
        if (!va) return 1;
        if (!vb) return -1;
        return vb - va;
    });
    for (var index in weapons) {
        $equip_list.append(weapons[index]);
    }
}

function sortWare() {
    var $ware_list = $('#ware_weapon_list');
    var weapons = [];
    $ware_list.children().each(function (index, ele) {
        weapons.push($(ele).detach());
    });
    weapons.sort(function (a, b) {
        var va = parseInt(a.find('.real_atk').html());
        var vb = parseInt(b.find('.real_atk').html());
        if (!va) return 1;
        if (!vb) return -1;
        return vb - va;
    });
    for (var index in weapons) {
        $ware_list.append(weapons[index]);
    }
}

const WEAPON_SKILLS =
    '<option value="0">無し</option>' +
    '<option value="1">攻刃(小)</option>' +
    '<option value="2">攻刃(中)</option>' +
    '<option value="3">攻刃(大)</option>' +
    '<option value="4">攻刃Ⅱ</option>' +
    '<option value="5">方陣攻刃</option>' +
    '<option value="6">方陣攻刃Ⅱ</option>' +
    '<option value="7">背水(小)</option>' +
    '<option value="8">背水(中)</option>' +
    '<option value="9">背水(大)</option>' +
    '<option value="10">方陣背水(小)</option>' +
    '<option value="11">方陣背水(中)</option>' +
    '<option value="12">方陣背水(大)</option>' +
    '<option value="13">バハ攻</option>' +
    '<option value="14">バハ攻HP</option>' +
    '<option value="15">アンノウンⅠ</option>' +
    '<option value="16">アンノウンⅡ</option>' +
    '<option value="21">其他ATK(中)</option>' +
    '<option value="17">其他ATK(大)</option>' +
    '<option value="18">方陣神威</option>' +
    '<option value="19">神威</option>' +
    '<option value="20">コスモス(武器)</option>';

function Weapon(obj) {
    this.atk = '';
    this.type = 0;
    this.skill1 = SKILL_NONE;
    this.skill2 = SKILL_NONE;
    this.slv = 1;
    this.name = '';

    if (obj) {
        this.atk = obj.atk;
        this.type = obj.type;
        this.skill1 = obj.skill1;
        this.skill2 = obj.skill2;
        this.slv = obj.slv;
        this.name = obj.name;
    }

    this.tojQObj = function () {
        var $ret = $($.sprintf('<tr class="weapon">' +
            '<td><input class="atk" value="%s"></td>' +
            '<td><select class="type">' +
            '<option value="0"></option>' +
            '<option value="1">剣</option>' +
            '<option value="2">短剣</option>' +
            '<option value="3">槍</option>' +
            '<option value="4">斧</option>' +
            '<option value="5">杖</option>' +
            '<option value="6">銃</option>' +
            '<option value="7">格闘</option>' +
            '<option value="8">弓</option>' +
            '<option value="9">楽器</option>' +
            '<option value="10">刀</option>' +
            '</select></td>' +
            '<td><select class="skill1">' +
            WEAPON_SKILLS +
            '</select></td>' +
            '<td><select class="skill2">' +
            WEAPON_SKILLS +
            '</select></td>' +
            '<td><select class="slv">' +
            '<option value="0">0</option>' +
            '<option value="1">1</option>' +
            '<option value="2">2</option>' +
            '<option value="3">3</option>' +
            '<option value="4">4</option>' +
            '<option value="5">5</option>' +
            '<option value="6">6</option>' +
            '<option value="7">7</option>' +
            '<option value="8">8</option>' +
            '<option value="9">9</option>' +
            '<option value="10">10</option>' +
            '<option value="11">11</option>' +
            '<option value="12">12</option>' +
            '<option value="13">13</option>' +
            '<option value="14">14</option>' +
            '<option value="15">15</option>' +
            '</select></td>' +
            '<td class="real_atk">%d</td>' +
            '<td><input class="name" value="%s"></td>' +
            '<td><button class="equip" onclick="equip(this);">装備変更</button></td>' +
            '</tr>',
            this.atk,
            0,
            this.name
        ));
        $ret.find('.type option[value=' + this.type + ']').attr("selected", "selected");
        $ret.find('.skill1 option[value=' + this.skill1 + ']').attr("selected", "selected");
        $ret.find('.skill2 option[value=' + this.skill2 + ']').attr("selected", "selected");
        $ret.find('.slv option[value=' + this.slv + ']').attr("selected", "selected");
        $ret.find('input,select').on('change', update);
        return $ret;
    };
}

Weapon.toJSON = function ($tr) {
    return {
        atk: $tr.find('.atk').val(),
        type: $tr.find('.type').val(),
        skill1: $tr.find('.skill1').val(),
        skill2: $tr.find('.skill2').val(),
        slv: $tr.find('.slv').val(),
        name: $tr.find('.name').val()
    };
};

function Buff(obj) {
    this.value = '';
    this.type = BUFF_NONE;
    this.name = '';

    if (obj) {
        this.value = obj.value;
        this.type = obj.type;
        this.name = obj.name;
    }

    this.tojQObj = function () {
        var $ret = $($.sprintf('<tr class="buff">' +
            '<td><input class="value" value="%s"></td>' +
            '<td colspan="4" style="text-align: left;"><select class="type">' +
            '<option value="0"></option>' +
            '<option value="1">通常攻UPバフ％</option>' +
            '<option value="2">属性攻UPバフ％</option>' +
            '<option value="3">其他攻UPバフ％</option>' +
            '<option value="4">敵防/属性DOWNデバフ％</option>' +
            '<option value="5">クリティカル＋％</option>' +
            '<option value="6">自攻DOWNデバフ％</option>' +
            '<option value="7">敵防UPバフ％</option>' +
            '<option value="8">敵ダメージカット</option>' +
            '<option value="9">アビリティ倍率</option>' +
            '<option value="10">敵素防御力指定</option>' +
            '</select></td>' +
            '<td class="real_damage">%d</td>' +
            '<td><input class="name" value="%s"></td>' +
            '<td><button class="equip" onclick="equip(this);">オン/オフ</button></td>' +
            '</tr>',
            this.value,
            this.name
        ));
        $ret.find('.type option[value=' + this.type + ']').attr("selected", "selected");
        $ret.find('input,select').on('change', update);
        return $ret;
    };
}

Buff.toJSON = function ($tr) {
    return {
        value: $tr.find('.value').val(),
        type: $tr.find('.type').val(),
        name: $tr.find('.name').val()
    };
};

$.fn.values = function (data) {
    var els = $(this).find(':input').get();

    if (typeof data != 'object') {
        // return all data
        data = {};

        $.each(els, function () {
            if (this.name && !this.disabled && (this.checked
                || /select|textarea/i.test(this.nodeName)
                || /text|hidden|password/i.test(this.type))) {
                data[this.name] = $(this).val();
            }
        });
        return data;
    } else {
        $.each(els, function () {
            if (this.name && data[this.name]) {
                if (this.type == 'checkbox' || this.type == 'radio') {
                    $(this).attr("checked", (data[this.name] == $(this).val()));
                } else {
                    $(this).val(data[this.name]);
                }
            }
        });
        return $(this);
    }
};

function saveCookies() {
    $.cookie('cloud_name', $('#cloud_name').val());
}

function loadCookies() {
    var cn = $.cookie('cloud_name');
    if (cn) {
        $('#cloud_name').val(cn);
    }
}
