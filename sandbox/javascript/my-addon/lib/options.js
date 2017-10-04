const options = document.querySelector('#options');

options.addEventListener('submit', (event) => {
  event.preventDefault();

  let data = new FormData(event.target);
  for (const [k, v] of data) {
    console.info('form_data', k, v);
  }
});
