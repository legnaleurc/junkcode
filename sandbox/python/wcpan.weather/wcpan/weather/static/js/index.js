(function () {
    'use strict';

    const inputSearchCity = document.querySelector('#input-search-city');


    function main () {
        inputSearchCity.addEventListener('keypress', (event) => {
            if (event.key === 'Enter') {
                ;
            }
        });
    }


    main();

})();
