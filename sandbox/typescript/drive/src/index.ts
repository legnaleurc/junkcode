import fs from "node:fs/promises";
import readline from "node:readline/promises";

import { google } from "googleapis";

const SCOPES = ["https://www.googleapis.com/auth/drive.file"];
const TOKEN_PATH = "token.json";

async function authorize(credentials) {
  const { client_secret, client_id, redirect_uris } = credentials.installed;
  const oAuth2Client = new google.auth.OAuth2(
    client_id,
    client_secret,
    redirect_uris[0],
  );

  try {
    const token = await readFileAsync(TOKEN_PATH);
    oAuth2Client.setCredentials(JSON.parse(token));
    return oAuth2Client;
  } catch (error) {
    return getNewToken(oAuth2Client);
  }
}

async function getNewToken(oAuth2Client) {
  const authUrl = oAuth2Client.generateAuthUrl({
    access_type: "offline",
    scope: SCOPES,
  });
  console.log("Authorize this app by visiting this URL:", authUrl);

  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  const code = await new Promise((resolve) => {
    rl.question("Enter the code from the page: ", (code) => {
      rl.close();
      resolve(code);
    });
  });

  const token = await oAuth2Client.getToken(code);
  oAuth2Client.setCredentials(token);

  await writeFileAsync(TOKEN_PATH, JSON.stringify(token));
  console.log("Token stored in", TOKEN_PATH);

  return oAuth2Client;
}

async function uploadFile(auth) {
  const drive = google.drive({ version: "v3", auth });
  const fileMetadata = {
    name: "YOUR_FILE_NAME.pdf", // The name you want to give the file on Google Drive
  };
  const media = {
    mimeType: "application/pdf", // Modify the MIME type according to your file
    body: fs.createReadStream("YOUR_FILE_TO_UPLOAD.pdf"),
  };

  const res = await drive.files.create({
    resource: fileMetadata,
    media: media,
    fields: "id",
  });

  console.log("File ID:", res.data.id);
}

async function main() {
  fs.readFile("YOUR_CLIENT_SECRET_FILE.json", async (err, content) => {
    if (err) {
      console.error("Error loading client secret file:", err);
      return;
    }

    const credentials = JSON.parse(content);
    const auth = await authorize(credentials);
    await uploadFile(auth);
  });
}
