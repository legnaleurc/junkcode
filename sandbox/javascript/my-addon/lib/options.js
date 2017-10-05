const options = document.querySelector('#options');

options.addEventListener('submit', (event) => {
  event.preventDefault();

  saveOptionsFromForm(event.target);
});

loadOptionsToForm(options);
