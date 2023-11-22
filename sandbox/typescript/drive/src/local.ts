import path from "node:path";
import fs from "node:fs/promises";
import { Stats } from "node:fs";
import crypto from "node:crypto";

import { FileMeta, FileService, ReadableFile, SnapshotService } from "./types";

export class LocalFileMeta implements FileMeta {
    private _path: string;
    private _stat: Stats;
    private _hash: string;
    private _root: string;

    constructor (path: string, stat: Stats, hash: string, root: string) {
        this._path = path;
        this._stat = stat;
        this._hash = hash;
        this._root = root;
    }

    get id(): string {
        return this._path;
    }

    get name(): string {
        return path.basename(this._path);
    }

    get parentId(): string {
        if (this._path === this._root) {
            return "";
        }
        return path.dirname(this._path);
    }

    get isTrashed(): boolean {
        return false;
    }

    get isFolder(): boolean {
        return this._stat.isDirectory();
    }

    get ctime(): Date {
        return this._stat.ctime;
    }

    get mtime(): Date {
        return this._stat.mtime;
    }

    get isImage(): boolean {
        return false;
    }

    get isVideo(): boolean {
        return false;
    }

    get mimeType(): string {
        return "application/octet-stream";
    }

    get hash(): string {
        return this._hash;
    }

    get size(): number {
        return this._stat.size;
    }

    get width(): number {
        return 0;
    }

    get height(): number {
        return 0;
    }

    get msDuration(): number {
        return 0;
    }

    get extra() {
        return {};
    }
}

async function createLocalFileMeta(path: string, root: string): Promise<FileMeta> {
    const stat = await fs.stat(path);
    const hash = await getFileHash(path, stat);
    return new LocalFileMeta(path, stat, hash, root);
}

async function getFileHash (path: string, stat: Stats): Promise<string> {
    if (!stat.isFile()) {
        return "";
    }
    const handle = await fs.open(path);
    const reader = new Promise<string>((resolve, reject) => {
        const hash = crypto.createHash("sha256");
        const fin = handle.createReadStream();
        fin.on("data", (chunk) => {
            hash.update(chunk);
        });
        fin.on("end", () => {
            resolve(hash.digest('hex'));
        });
        fin.on("error", reject);
    });
    try {
        return reader;
    } finally {
        await handle.close();
    }
}

export class LocalFileService implements FileService {
    private _root: string;

    constructor (rootPath: string) {
        this._root = rootPath;
    }

    async fetchInitialCursor(): Promise<string> {
        return "0";
    }

    async fetchRoot(): Promise<FileMeta> {
        return await createLocalFileMeta(this._root, this._root);
    }

    async *fetchChangeList(cursor: string) {
        ;
    }
}

export class LocalSnapshotService implements SnapshotService {
    private _cursor: string;
    private _root: FileMeta;

    constructor () {
        this._cursor = "0";
        this._root = null as unknown as FileMeta;
    }

    async getCurrentCursor(): Promise<string> {
        return this._cursor;
    }

    async setRoot(root: FileMeta): Promise<void> {
        this._root = root;
    }

    async getRoot(): Promise<FileMeta> {
        return this._root;
    }
}
