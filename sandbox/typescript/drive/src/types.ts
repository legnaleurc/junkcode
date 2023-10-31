export type FileMeta = {
  id: string;
  parentId: string;
  name: string;
  isTrashed: boolean;
  ctime: Date;
  mtime: Date;
  extra: Record<string, unknown>;
  isFolder: boolean;
  isImage: boolean;
  isVideo: boolean;
  mimeType: string;
  hash: string;
  size: number;
  width: number;
  height: number;
  msDuration: number;
};

export interface Drive {
  sync(): Promise<void>;
  walk(
    root: FileMeta,
  ): AsyncGenerator<[FileMeta, Array<FileMeta>, Array<FileMeta>], void, void>;
}

type RemoveAction = [true, string];
type UpdateAction = [false, FileMeta];
type ChangeAction = RemoveAction | UpdateAction;
type ChangeCursor = [string, ...ChangeAction];

export interface Service {
  download(file: FileMeta): Promise<ReadableFile>;
  upload(file: FileMeta): Promise<WritableFile>;
  create_folder(name: string, parent: FileMeta): Promise<FileMeta>;

  move(
    file: FileMeta,
    options: Partial<{ name: string; parentId: string }>,
  ): Promise<void>;
  trash(file: FileMeta): Promise<void>;

  fetchInitialCursor(): Promise<string>;
  fetchRootFile(): Promise<FileMeta>;
  fetchChangeList(cursor: string): AsyncGenerator<ChangeCursor, void, void>;
}

export interface RandomAccessFile {
  open(): Promise<void>;
  close(): Promise<void>;
  seek(offset: number): Promise<void>;
  tell(): Promise<number>;
}

export interface ReadableFile extends RandomAccessFile {
  read(numBytes: number): Promise<Uint8Array | null>;
}

export interface WritableFile extends RandomAccessFile {
  write(data: Uint8Array): Promise<number>;
  flush(): Promise<void>;
}

export interface Snapshot {
  getMetadata(key: string): Promise<string>;
  setMetadata(key: string, value: string): Promise<void>;

  getRootId(): Promise<string>;

  getFileById(fileId: string): Promise<FileMeta>;
  getFileByAbsolutePath(absolutePath: string): Promise<FileMeta>;

  listFilesInFolder(fileId: string): Promise<FileMeta[]>;
  listFilesInTrash(): Promise<FileMeta[]>;

  getFileByNameInFolder(fileName: string, parentId: string): Promise<FileMeta>;

  searchFilesByName(regex: RegExp): Promise<FileMeta[]>;

  applyChangeList(
    changeList: Array<ChangeAction>,
    cursor: string,
  ): Promise<void>;
}
