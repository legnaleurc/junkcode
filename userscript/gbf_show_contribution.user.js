// ==UserScript==
// @name         GBF show contribution
// @namespace    sandbox
// @match        http://game.granbluefantasy.jp/*
// @grant        unsafeWindow
// @grant        GM_addStyle
// ==/UserScript==

if (location.hash.indexOf('#raid_multi/') !== 0) {
    return;
}

main();


async function main () {
    GM_addStyle(`
        #pop-scroll .pop-alliance-window .lis-user .ico-quest-host {
            top: -1px;
        }
    `);
    const subWidget = await waitDOM('#pop-scroll', document);
    listenPopUp('.pop-alliance-window', subWidget, showContribution);
}


function waitDOM (selector, parent) {
    return new Promise((resolve) => {
        const observer = new MutationObserver(() => {
            const node = parent.querySelector(selector);
            if (!node) {
                return;
            }
            observer.disconnect();
            resolve(node);
        });
        observer.observe(document, {
            childList: true,
            subtree: true,
        });
    });
}


function listenPopUp (selector, parent, handler) {
    const observer = new MutationObserver(() => {
        const node = parent.querySelector(selector);
        if (node) {
            handler(node);
        }
    });
    observer.observe(parent, {
        childList: true,
    });
}


function showContribution (dialog) {
    const table = createTable();
    for (const child of dialog.children) {
        const userID = child.dataset.id;
        let point = table.get(userID);
        if (typeof point === 'undefined') {
            point = -1;
        }
        appendNumber(child, userID, point);
    }
}


function createTable () {
    const mvp = unsafeWindow.stage.pJsnData.multi_raid_member_info;
    const table = new Map();
    for (const player of mvp) {
        const iPoint = player.point * 1;
        table.set(player.user_id, iPoint);
    }
    return table;
}


function appendNumber (widget, userID, point) {
    const fragment = document.createDocumentFragment();
    fragment.appendChild(createLabel(`${point.toLocaleString()} pt`));

    widget.title = userID;
    widget.appendChild(fragment);
}


function createLabel (text) {
    let label = document.createElement('div');
    label.classList.add('txt-rank');
    label.style.backgroundColor = 'black';
    label.style.paddingTop = '0px';
    label.style.marginTop = '2px';
    label.style.zIndex = '-1';
    label.textContent = text;
    return label;
}
