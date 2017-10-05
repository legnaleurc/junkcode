const options = document.querySelector('#options');

options.addEventListener('submit', (event) => {
  event.preventDefault();

  saveOptionsFromForm(event.target);
});

(async () => {
  const opts = await loadOptions();
  for (const input of options.elements) {
    if (opts.hasOwnProperty(input.name)) {
      input.value = opts[input.name];
    }
  }
})();
