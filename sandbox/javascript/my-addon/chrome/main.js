const REQUEST_FILTERS = [
  'main_frame',
  'sub_frame',
  // 'script',
];
const EMPTY_SCRIPT = '// __CENSORED__';


async function main () {
  await browser.webRequest.handlerBehaviorChanged();

  browser.webRequest.onBeforeRequest.addListener(before, {
    urls: ['<all_urls>'],
    types: REQUEST_FILTERS,
  }, ['blocking']);
}


function before (details) {
  let isScript = details.type === 'script';
  let filter = browser.webRequest.filterResponseData(details.requestId);
  let decoder = new TextDecoder("utf-8");
  let encoder = new TextEncoder();

  filter.ondata = (event) => {
    let content = decoder.decode(event.data, {
      stream: true,
    });
    if (isScript) {
      content = filterScript(content);
    } else {
      content = filterDocument(content);
    }
    content = encoder.encode(content);
    filter.write(content);
    filter.disconnect();
  };
}


function filterScript (usScript) {
  let pattern = /setInterval\(function\(\)\{_0x\w+\(\);\},0xfa0\)/;
  if (pattern.test(usScript)) {
    return EMPTY_SCRIPT;
  }
  return usScript;
}


function filterDocument (usDocument) {
  let parser = new DOMParser();
  let doc = parser.parseFromString(usDocument, 'text/html');
  let scripts = doc.querySelectorAll('script');
  for (script of scripts) {
    script.textContent = filterScript(script.textContent);
  }
  return '<!DOCTYPE html>' + doc.documentElement.outerHTML;
}


main().catch((e) => {
  console.warn('main fatal', e);
});
