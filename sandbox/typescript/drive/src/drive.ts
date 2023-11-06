import { Drive, FileService, SnapshotService } from "./types";

type CreateDriveParameters = {
    createFileService: () => Promise<FileService>;
    createSnapshotService: () => Promise<SnapshotService>;
};
export async function createDrive(params: CreateDriveParameters): Promise<Drive> {
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

    async sync() {}

    async * walk() {}
}
