export const SORT_BY_NAME_ASC = 'SORT_BY_NAME_ASC';
export const SORT_BY_MTIME_ASC = 'SORT_BY_MTIME_ASC';
export const SORT_BY_MTIME_DES = 'SORT_BY_MTIME_DES';


function sortByName (a, b) {
  if (a.name < b.name) {
    return -1;
  }
  if (a.name > b.name) {
    return 1;
  }
  return 0;
}


function sortByDate (a, b) {
  if (a.modified < b.modified) {
    return -1;
  }
  if (a.modified > b.modified) {
    return 1;
  }
  return 0;
}


const TABLE = {
  SORT_BY_NAME_ASC: sortByName,
  SORT_BY_MTIME_ASC: sortByDate,
  SORT_BY_MTIME_DES: (a, b) => (-1 * sortByDate(a, b)),
};


export function getCompareFunction (key) {
  const fn = TABLE[key];
  if (!fn) {
    return sortByName;
  }
  return fn;
}
