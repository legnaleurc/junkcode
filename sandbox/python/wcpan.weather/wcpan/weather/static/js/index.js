(function () {
    'use strict';

    const selectionCountry = document.querySelector('#selection-country');
    const selectionCity = document.querySelector('#selection-city');
    const displayIcon = document.querySelector('#display-icon');
    const displayTemp = document.querySelector('#display-temp');


    async function main () {
        selectionCountry.addEventListener('change', async (event) => {
            await updateCities();
        });
        selectionCity.addEventListener('change', async (event) => {
            await updateWeather();
        });

        await updateCities();
    }


    async function updateCities () {
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


    async function updateWeather () {
        let cityID = selectionCity.value;
        let weather = await fetch(`/api/v1/weather/${cityID}`);
        weather = await weather.json();

        displayIcon.textContent = weather.icon;
        displayTemp.textContent = weather.temp;
    }


    main().catch((e) => {
        console.error(e);
    });

})();
