// TODO multi-hosts?
const kCsrfTokenKey = 'X-Transmission-Session-Id';
let gCurrentCsrfToken = null;


function getDefaultValue () {
  return {
    version: 1,
    url: '',
    username: '',
    password: '',
    'add-paused': false,
  };
}


async function loadOptions () {
  let opts = null;
  try {
    opts = await browser.storage.local.get('options');
  } catch (e) {
    opts = null;
  }
  if (opts) {
    opts = opts.options;
  }
  if (!opts) {
    opts = getDefaultValue();
  }
  // TODO migration
  if (opts.version !== 1) {
    throw new Error('incompatible version');
  }
  delete opts.version;
  return opts;
}


async function saveOptions (opts) {
  await browser.storage.local.set({
    options: opts,
  });
}


async function loadOptionsToForm (form) {
  const opts = await loadOptions();
  for (const input of form.elements) {
    if (opts.hasOwnProperty(input.name)) {
      if (input.type === 'checkbox') {
        input.checked = opts[input.name];
      } else {
        input.value = opts[input.name];
      }
    }
  }
}


async function saveOptionsFromForm (form) {
  const opts = Array.prototype.reduce.call(form.elements, (rv, input) => {
    if (input.name === 'version') {
      rv.version = parseInt(input.value, 10);
      return rv;
    }
    switch (input.type) {
      case 'text':
      case 'password':
        rv[input.name] = input.value;
        break;
      case 'checkbox':
        rv[input.name] = input.checked;
        break;
      default:
        break;
    }
    return rv;
  }, {});
  await saveOptions(opts);
}


async function sendToTransmission (torrentURL) {
  const opts = await loadOptions();

  const headers = new Headers();
  headers.append('Content-Type', 'application/json');
  if (opts.username && opts.password) {
    const authToken = btoa(`${opts.username}:${opts.password}`);
    headers.append('Authorization', `Basic ${authToken}`);
  }
  if (gCurrentCsrfToken) {
    headers.append(kCsrfTokenKey, gCurrentCsrfToken);
  }

  const args = {
    method: 'torrent-add',
    arguments: {
      filename: torrentURL,
      paused: opts['add-paused'],
    },
  };

  const request = new Request(opts.url, {
    method: 'POST',
    headers,
    body: JSON.stringify(args),
    mode: 'cors',
    credentials: 'include',
  });

  let rv = await fetch(request);
  if (rv.status === 409) {
    if (rv.headers.has(kCsrfTokenKey)) {
      gCurrentCsrfToken = rv.headers.get(kCsrfTokenKey);
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


class PromptServer {

  constructor (tabId) {
    this._tabId = tabId;
  }

  async setMessage (message) {
    await browser.tabs.sendMessage(this._tabId, {
      topic: 'show-prompt',
      args: {
        message,
      },
    });
  }

  async close () {
    await browser.tabs.sendMessage(this._tabId, {
      topic: 'close-prompt',
    });
  }

}
