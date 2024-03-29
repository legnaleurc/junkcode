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

type AG<T> = AsyncGenerator<T, void, void>;

type RemoveAction = [true, string];
type UpdateAction = [false, FileMeta];
type ChangeAction = RemoveAction | UpdateAction;
type ChangeCursor = [Array<ChangeAction>, string];

export interface FileService {
  download(file: FileMeta): Promise<ReadableFile>;
  upload(file: FileMeta): Promise<WritableFile>;
  createFolder(name: string, parent: FileMeta): Promise<FileMeta>;

  move(
    file: FileMeta,
    options: Partial<{ name: string; parentId: string }>,
  ): Promise<void>;
  trash(file: FileMeta): Promise<void>;

  fetchInitialCursor(): Promise<string>;
  fetchRoot(): Promise<FileMeta>;
  fetchChangeList(cursor: string): AG<ChangeCursor>;
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

export interface SnapshotService {
  getCurrentCursor(): Promise<string>;
  getRoot(): Promise<FileMeta>;
  setRoot(root: FileMeta): Promise<void>;

  getFileById(fileId: string): Promise<FileMeta>;
  getFileByAbsolutePath(absolutePath: string): Promise<FileMeta>;

  getChildren(fileId: string): Promise<FileMeta[]>;
  listFilesInTrash(): Promise<FileMeta[]>;

  getFileByNameInFolder(fileName: string, parentId: string): Promise<FileMeta>;

  searchFilesByName(regex: RegExp): Promise<FileMeta[]>;

  applyChangeList(
    changeList: Array<ChangeAction>,
    cursor: string,
  ): Promise<void>;
}

type WalkEntry = [FileMeta, Array<FileMeta>, Array<FileMeta>];

export interface Drive {
  getRoot(): Promise<FileMeta>;
  getFileById(id: string): Promise<FileMeta>;
  getChildren(file: FileMeta): Promise<Array<FileMeta>>;

  sync(): AG<ChangeAction>;
  walk(root: FileMeta): AG<WalkEntry>;
}
