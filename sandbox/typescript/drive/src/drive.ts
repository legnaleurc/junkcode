import type { Drive, FileMeta, FileService, SnapshotService } from "./types";

type CreateDriveParameters = {
  createFileService: () => Promise<FileService>;
  createSnapshotService: () => Promise<SnapshotService>;
};
export async function createDrive(
  params: CreateDriveParameters,
): Promise<Drive> {
  const fileService = await params.createFileService();
  const snapshotService = await params.createSnapshotService();
  return new DefaultDrive({
    fileService,
    snapshotService,
  });
}

type DefaultDriveConstructorParameters = {
  fileService: FileService;
  snapshotService: SnapshotService;
};
class DefaultDrive implements Drive {
  private fileService: FileService;
  private snapshotService: SnapshotService;

  constructor(params: DefaultDriveConstructorParameters) {
    this.fileService = params.fileService;
    this.snapshotService = params.snapshotService;
  }

  async getRoot(): Promise<FileMeta> {
    const rootId = await this.snapshotService.getRootId();
    const root = await this.getFileById(rootId);
    return root;
  }

  async getFileById(id: string): Promise<FileMeta> {
    const file = await this.snapshotService.getFileById(id);
    return file;
  }

  async getChildren(file: FileMeta): Promise<Array<FileMeta>> {
    const children = await this.snapshotService.getChildren(file.id);
    return children;
  }

  async resolve() {}

  async *sync() {
    const initialCursor = await this.fileService.fetchInitialCursor();
    let cursor = initialCursor;
    try {
      cursor = await this.snapshotService.getCurrentCursor();
    } catch (e: unknown) {
      console.warn(e);
    }
    // no data, reset root node
    if (cursor === initialCursor) {
      const root = await this.fileService.fetchRoot();
      await this.snapshotService.setRoot(root);
    }

    for await (const [changeList, next] of this.fileService.fetchChangeList(cursor)) {
      await this.snapshotService.applyChangeList(changeList, next);
      for (const change of changeList) {
        yield change;
      }
    }
  }

  async *walk() {}
}
