// ==UserScript==
// @name         GBF sort supporters
// @namespace    sandbox
// @version      0.1
// @match        http://game.granbluefantasy.jp/*
// @grant        none
// ==/UserScript==

'use strict';

const SCORE_TABLE = [
    // free
    {
        'カグヤ': [80, 90, 0, 0],
        'シルフィードベル': [30, 40, 0, 0],
        'ジ・オーダー・グランデ': [10, 20, 0, 0],
        'ノビヨ': [0, 60, 0, 0],
        'ブラックラビット': [50, 70, 0, 0],
        'ホワイトラビット': [50, 70, 0, 0],
    },
    // fire
    {
        'アテナ': [0, 10, 20, 0],
        'アグニス・ディヴィジョン': [0, 30, 0, 0],
        'アグニス': [0, 40, 0, 0],
        'ミカエル': [50, 60, 0, 0],
        '白純の霊蝶 シルフ': [60, 70, 0, 0],
        'シヴァ': [70, 80, 0, 0],
    },
    // water
    {
        'ヴァルナ・ディヴィジョン': [0, 10, 0, 0],
        'エウロペ': [20, 30, 0, 0],
        'ヴァルナ': [0, 40, 0, 0],
        'リヴァイアサン・マグナ': [0, 0, 0, 0],
    },
    // earth
    {
        'ウリエル': [60, 70, 0, 0],
        'ゴッドガード・ブローディア': [70, 80, 0, 0],
        'ユグドラシル・マグナ': [0, 80, 90, 0],
        'ティターン': [0, 50, 0, 0],
        'ティターン・ディヴィジョン': [0, 40, 0, 0],
        'テスカトリポカ': [0, 0, 0, 0],
    },
    // wind
    {
        'アナト': [0, 10, 0, 0],
        'グリームニル': [40, 50, 0, 0],
        'ラファエル': [20, 30, 0, 0],
        'ティアマト・マグナ': [0, 0, 50, 0],
        '愛と戦いの神 アナト': [40, 50, 0, 0],
    },
    // light
    {
        'シュヴァリエ・マグナ': [0, 40, 50, 0],
        'ルシフェル': [10, 20, 30, 40],
        'ゼウス': [0, 30, 0, 0],
        'ゼウス・ディヴィジョン': [0, 20, 0, 0],
    },
    // dark
    {
        'セレスト・マグナ': [0, 40, 50, 0],
        'バハムート': [10, 20, 30, 40],
    },
];

const URLS = [
    '#quest/supporter',
    '#event/teamraid',
];


async function main () {
    await handleSupporters();
    window.addEventListener('hashchange', handleSupporters);
}


async function handleSupporters () {
    if (!shouldSort()) {
        return;
    }
    await wait();
    const elementalList = getElementalList();
    elementalList.forEach((block, index) => {
        let list = detachChildren(index, block);
        list = sortByScore(list);
        returnChildren(block, list);
    });
}


function shouldSort () {
    return URLS.some((h) => {
        return location.hash.indexOf(h) === 0;
    });
}


function getElementalList () {
    const list = document.querySelectorAll('.prt-supporter-attribute');
    return list;
}


function detachChildren (type, parentBlock) {
    const list = [];
    for (let el = parentBlock.firstElementChild;
         el;
         el = parentBlock.firstElementChild)
    {
        el.remove();
        list.push({
            score: computeScore(type, el),
            view: el,
        });
    }
    return list;
}


// binary score priority: name[rank] + isFriend
function computeScore (type, el) {
    let tmp = el.querySelector('.prt-supporter-summon');
    const name = tmp.childNodes[2].nodeValue.trim();
    tmp = el.querySelector('.prt-supporter-name');
    const isFriend = tmp.classList.contains('ico-friend') ? 0 : 1;

    const fn = SCORE_TABLE[type][name];
    if (!fn) {
        return 0 + isFriend;
    }

    let rank = 0;
    if (el.querySelector('.bless-rank1-style')) {
        rank = 1;
    } else if (el.querySelector('.bless-rank2-style')) {
        rank = 2;
    } else if (el.querySelector('.bless-rank3-style')) {
        rank = 3;
    }

    return fn[rank] + isFriend;
}


function sortByScore (list) {
    return list.sort((l, r) => {
        if (l.score > r.score) {
            return -1;
        }
        if (l.score < r.score) {
            return 1;
        }
        return 0;
    });
}


function returnChildren (parentBlock, list) {
    const tmp = document.createDocumentFragment();
    for (const _ of list) {
        tmp.appendChild(_.view);
    }
    parentBlock.appendChild(tmp);
}


function wait () {
    return new Promise((resolve) => {
        if (areSupportersReady()) {
            resolve();
            return;
        }

        const INTERVAL = 200;
        const handle = setInterval(() => {
            if (!areSupportersReady()) {
                return;
            }
            clearInterval(handle);
            // yield for a short time to breath
            setTimeout(resolve, INTERVAL);
        }, INTERVAL);
    });
}


function areSupportersReady () {
    // observe any supporters
    const el = document.querySelector('.prt-supporter-attribute');
    return (el && el.childElementCount > 0);
}


main().catch((e) => {
    console.error(e);
});
