async function loadOptions () {
  try {
    const opts = await chrome.storage.local.get('options');
    return opts.options;
  } catch (e) {
    return {};
  }
}


async function saveOptionsFromForm (form) {
  let data = new FormData(form);
  for (const [k, v] of data) {
    console.info('form_data', k, v);
  }
}
