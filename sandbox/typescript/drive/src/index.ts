import fs from "node:fs/promises";
import readline from "node:readline/promises";

import { google } from "googleapis";
import { OAuth2Client } from "google-auth-library";

const SCOPES = ["https://www.googleapis.com/auth/drive.file"];
const TOKEN_PATH = "token.json";

type Credential = {
  installed: {
    client_secret: string;
    client_id: string;
    redirect_uris: string[];
  };
};

async function authorize(credentials: Credential) {
  const { client_secret, client_id, redirect_uris } = credentials.installed;
  const oAuth2Client = new google.auth.OAuth2(
    client_id,
    client_secret,
    redirect_uris[0],
  );

  try {
    const token = await fs.readFile(TOKEN_PATH, { encoding: "utf-8" });
    oAuth2Client.setCredentials(JSON.parse(token));
    return oAuth2Client;
  } catch (error) {
    return getNewToken(oAuth2Client);
  }
}

async function getNewToken(oAuth2Client: OAuth2Client) {
  const authUrl = oAuth2Client.generateAuthUrl({
    access_type: "offline",
    scope: SCOPES,
  });
  console.log("Authorize this app by visiting this URL:", authUrl);

  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  const code = await rl.question("Enter the code from the page: ");

  const token = await oAuth2Client.getToken(code);
  oAuth2Client.setCredentials(token.tokens);

  await fs.writeFile(TOKEN_PATH, JSON.stringify(token), { encoding: "utf-8" });
  console.log("Token stored in", TOKEN_PATH);

  return oAuth2Client;
}

async function uploadFile(auth: OAuth2Client) {
  const drive = google.drive({ version: "v3", auth });
  const fileMetadata = {
    name: "YOUR_FILE_NAME.pdf", // The name you want to give the file on Google Drive
  };
  const fin = await fs.open("YOUR_FILE_TO_UPLOAD.pdf");
  const media = {
    mimeType: "application/pdf", // Modify the MIME type according to your file
    body: fin.createReadStream(),
  };

  const res = await drive.files.create({
    requestBody: fileMetadata,
    media: media,
    fields: "id",
  });

  console.log("File ID:", res.data.id);
}

async function downloadFile(auth: OAuth2Client, fileId: string, range: { start: number; end: number; }) {
  const drive = google.drive({ version: 'v3', auth });

  const options = {
    headers: {
      'Range': `bytes=${range.start}-${range.end}`,
    },
  };

  return drive.files.get(
    { fileId, alt: 'media' },
    options
  ).then((res) => {
    if (res.status !== 200) {
      throw new Error(`Failed to download file. Status: ${res.status}`);
    }
    return res.data;
  });
}

async function main() {
  const content = await fs.readFile("YOUR_CLIENT_SECRET_FILE.json", { encoding: "utf-8" });
  const credentials = JSON.parse(content);
  const auth = await authorize(credentials);
  await uploadFile(auth);
}

main();
