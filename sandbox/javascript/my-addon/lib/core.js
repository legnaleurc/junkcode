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
    opts = opts.options;
  } catch (e) {
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
