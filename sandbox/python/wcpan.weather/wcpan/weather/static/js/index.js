(function () {
    'use strict';

    const selectionCountry = document.querySelector('#selection-country');
    const selectionCity = document.querySelector('#selection-city');


    async function main () {
        let countryID = selectionCountry.value;
        let cities = await fetch(`/api/v1/country/${countryID}/city`);
        cities = await cities.json();
    }


    main().catch((e) => {
        console.error(e);
    });

})();
