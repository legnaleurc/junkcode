const process = require('process');

const octokit = require('@octokit/rest');


async function main () {
  const session = octokit();

  let rv = await getContributors(session);
  rv = rv.filter((v, i, a) => {
    return v.login !== 'legnaleurc';
  });

  console.info(rv);
  return;
  rv = rv.map((v, i, a) => {
    return getLoginName(session, v.login);
  });
  rv = await Promise.all(rv);
  rv.sort((l, r) => {
    let a = l.login.toLowerCase();
    let b = r.login.toLowerCase();
    if (a < b) {
      return -1;
    }
    if (a > b) {
      return 1;
    }
    return 0;
  });

  for (let {login, name} of rv) {
    let msg = `* [${name}](https://github.com/${login})`;
    if (login === 'devnoname120') {
      msg += ', the author of [AntiAdware](https://github.com/handyuserscripts/antiadware#readme)';
    }
    process.stdout.write(msg + '\n');
  }
}


async function getContributors (session) {
  let rv = await session.repos.getContributors({
    owner: 'adsbypasser',
    repo: 'adsbypasser',
  });
  let data = rv.data;
  while (session.hasNextPage(rv)) {
    rv = await session.getNextPage(rv);
    data = data.concat(rv.data);
  }
  return data;
}


async function getLoginName (session, login) {
  let name = login;
  try {
    let rv = await session.users.getForUser({
      username: login,
    });
    name = rv.data.name || login;
  } catch (e) {
    // do nothing
    console.warn(e);
  }
  return {
    login,
    name,
  };
}


main().catch((e) => {
  console.error(e);
});
