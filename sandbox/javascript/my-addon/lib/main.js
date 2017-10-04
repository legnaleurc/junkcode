const URL_ = '';
const username = '';
const password = '';
const addPaused = true;
const enableForAllLinks = true;
const troubleshooting = false;
const tooltipPosition = 1;

const csrfTokenKey = 'X-Transmission-Session-Id';
let currentCsrfToken = null;


chrome.contextMenus.create({
  id: 'send-to-transmission',
  title: 'Send To Transmission',
  contexts: ['link'],
});

chrome.contextMenus.onClicked.addListener((info, tab) => {
  if (info.menuItemId === 'send-to-transmission') {
    sendToTransmission(info.linkUrl)
      .then((rv) => {
        console.info('after sendToTransmission', rv);
      })
      .catch((e) => {
        console.error('after sendToTransmission', e);
      });
  }
});


async function sendToTransmission (torrentURL) {
  const headers = new Headers();
  const authToken = btoa(`${username}:${password}`);
  headers.append('Authorization', `Basic ${authToken}`);
  headers.append('Content-Type', 'application/json');
  if (currentCsrfToken) {
    headers.append(csrfTokenKey, currentCsrfToken);
  }

  const args = {
    method: 'torrent-add',
    arguments: {
      filename: torrentURL,
      paused: addPaused,
    },
  };

  const request = new Request(URL_, {
    method: 'POST',
    headers,
    body: JSON.stringify(args),
    mode: 'cors',
    credentials: 'include',
  });

  let rv = await fetch(request);
  if (rv.status === 409) {
    if (rv.headers.has(csrfTokenKey)) {
      currentCsrfToken = rv.headers.get(csrfTokenKey);
      return sendToTransmission(torrentURL);
    }
  }
  if (!rv.ok) {
    throw new Error(`request error: ${rv.status}`);
  }
  rv = await rv.json();
  if (rv.result !== 'success') {
    throw new Error(`transmission error: ${rv.result}`);
  }
  return rv;
}
