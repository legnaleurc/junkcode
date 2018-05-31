(function () {
    'use strict';

    const selectionCountry = document.querySelector('#selection-country');
    const selectionCity = document.querySelector('#selection-city');


    async function main () {
        let countryID = selectionCountry.value;
        let cities = await fetch(`/api/v1/country/${countryID}/city`);
        cities = await cities.json();

        while (selectionCity.firstChild) {
            selectionCity.removeChild(selectionCity.firstChild);
        }
        for (let {id, name} of cities) {
            let o = document.createElement('option');
            o.value = id;
            o.textContent = name;
            selectionCity.appendChild(o);
        }
    }


    main().catch((e) => {
        console.error(e);
    });

})();
