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
    opts = await chrome.storage.local.get('options');
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
  ;
}


async function saveOptionsFromForm (form) {
  let data = new FormData(form);
  for (const [k, v] of data) {
    console.info('form_data', k, v);
  }
}
