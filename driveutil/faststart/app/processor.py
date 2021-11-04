import asyncio
import json
import os.path
import pathlib
import shutil
import subprocess

from wcpan.drive.core.drive import Drive, Node, MediaInfo
from wcpan.drive.core.util import upload_from_local


H264_PRESET = 'veryfast'
H264_CRF = '18'


class VideoProcessor(object):

    def __init__(self, base_url: str, work_folder: str, drive: Drive, node: Node):
        self.base_url = base_url
        self.work_folder = pathlib.Path(work_folder)
        self.drive = drive
        self.node = node
        # implies mp4
        self.is_faststart = True
        self.is_h264 = True
        self.is_aac = True
        self.is_mp3 = True

    @property
    def http_url(self) -> str:
        return f'{self.base_url}/api/v1/nodes/{self.node.id_}/stream/{self.node.name}'

    async def prepare_codec_info(self):
        raise NotImplementedError()

    @property
    def codec_command(self) -> list[str]:
        raise NotImplementedError()

    @property
    def transcoded_file_name(self) -> str:
        raise NotImplementedError()

    async def update_codec_from_ffmpeg(self):
        out, err = await shell_pipe([
            'ffprobe',
            '-v', 'trace',
            '-show_streams',
            '-print_format', 'json',
            '-i', self.http_url,
        ])
        data = json.loads(out)
        data = data['streams']
        for stream in data:
            if stream['codec_type'] == 'audio':
                self.is_aac = stream['codec_name'] == 'aac'
                self.is_mp3 = stream['codec_name'] == 'mp3'
            elif stream['codec_type'] == 'video':
                self.is_h264 = stream['codec_name'] == 'h264'
        self.is_faststart = err.find('bytes read, 0 seeks') >= 0

    def _is_skippable(self):
        return self.is_faststart and self.is_h264 and (self.is_aac or self.is_mp3)

    async def __call__(self):
        await self.prepare_codec_info()
        if self._is_skippable():
            return

        self._dump_info()

        transcode_command = self._get_transcode_command()

        print(transcode_command)

        exit_code = await shell_call(transcode_command)
        if exit_code != 0:
            print('ffmpeg failed')
            return
        await self._delete_remote()
        await self._upload()
        self._delete_local()

    def _get_transcode_command(self):
        main_cmd = ['ffmpeg', '-nostdin', '-y']
        input_cmd = ['-i', self.http_url]
        codec_cmd = self.codec_command
        output_folder = self.work_folder / self.node.id_
        output_folder.mkdir(exist_ok=True)
        output_path = output_folder / self.transcoded_file_name
        cmd = main_cmd + input_cmd + codec_cmd + [output_path]
        return cmd

    async def _delete_remote(self):
        await self.drive.trash_node(self.node)

    async def _upload(self):
        output_folder = self.work_folder / self.node.id_
        output_path = output_folder / self.transcoded_file_name
        parent_node = await self.drive.get_node_by_id(self.node.parent_id)
        media_info = MediaInfo.video(self.node.video_width, self.node.video_height, self.node.video_ms_duration)
        await upload_from_local(self.drive, parent_node, output_path, media_info)

    def _delete_local(self):
        output_folder = self.work_folder / self.node.id_
        shutil.rmtree(output_folder)

    def _dump_info(self):
        print(f'node id: {self.node.id_}')
        print(f'node name: {self.node.name}')
        print(f'is faststart: {self.is_faststart}')
        print(f'is h264: {self.is_h264}')
        print(f'is aac: {self.is_aac}')
        print(f'is mp3: {self.is_mp3}')


class MP4Processer(VideoProcessor):

    def __init__(self, base_url: str, work_folder: str, drive: Drive, node: Node):
        super().__init__(base_url, work_folder, drive, node)

    async def prepare_codec_info(self):
        await self.update_codec_from_ffmpeg()

    @property
    def codec_command(self):
        if self.is_h264:
            vc = ['-c:v', 'copy']
        else:
            vc = ['-c:v', 'libx264', '-crf', H264_CRF, '-preset', H264_PRESET]
        if self.is_aac or self.is_mp3:
            ac = ['-c:a', 'copy']
        else:
            ac = []
        return ac + vc + ['-movflags', '+faststart']

    @property
    def transcoded_file_name(self):
        return self.node.name


class MaybeH264Processer(VideoProcessor):

    def __init__(self, base_url: str, work_folder: str, drive: Drive, node: Node):
        super().__init__(base_url, work_folder, drive, node)

    async def prepare_codec_info(self):
        await self.update_codec_from_ffmpeg()
        self.is_faststart = False

    @property
    def codec_command(self):
        if self.is_h264:
            vc = ['-c:v', 'copy']
        else:
            vc = ['-c:v', 'libx264', '-crf', H264_CRF, '-preset', H264_PRESET]
        if self.is_aac or self.is_mp3:
            ac = ['-c:a', 'copy']
        else:
            ac = []
        return ac + vc + ['-movflags', '+faststart']

    @property
    def transcoded_file_name(self):
        name, ext = os.path.splitext(self.node.name)
        return name + '.mp4'


class MKVProcesser(VideoProcessor):

    def __init__(self, base_url: str, work_folder: str, drive: Drive, node: Node):
        super().__init__(base_url, work_folder, drive, node)

    async def prepare_codec_info(self):
        await self.update_codec_from_ffmpeg()
        self.is_faststart = False

    @property
    def codec_command(self):
        if self.is_h264:
            vc = ['-c:v', 'copy']
        else:
            vc = ['-c:v', 'libx264', '-crf', H264_CRF, '-preset', H264_PRESET]
        if self.is_aac or self.is_mp3:
            ac = ['-c:a', 'copy']
        else:
            ac = []
        return ac + vc + ['-movflags', '+faststart']

    @property
    def transcoded_file_name(self):
        name, ext = os.path.splitext(self.node.name)
        return name + '.mp4'


class NeverH264Processer(VideoProcessor):

    def __init__(self, base_url: str, work_folder: str, drive: Drive, node: Node):
        super().__init__(base_url, work_folder, drive, node)

    async def prepare_codec_info(self):
        await self.update_codec_from_ffmpeg()
        self.is_faststart = False

    @property
    def codec_command(self):
        if self.is_h264:
            vc = ['-c:v', 'copy']
        else:
            vc = ['-c:v', 'libx264', '-crf', H264_CRF, '-preset', H264_PRESET]
        if self.is_aac or self.is_mp3:
            ac = ['-c:a', 'copy']
        else:
            ac = []
        return ac + vc + ['-movflags', '+faststart']

    @property
    def transcoded_file_name(self):
        name, ext = os.path.splitext(self.node.name)
        return name + '.mp4'


async def shell_pipe(cmd_list: list[str]):
    p = await asyncio.create_subprocess_exec(*cmd_list, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = await p.communicate()
    return out.decode('utf-8'), err.decode('utf-8')


async def shell_call(cmd_list: list[str]):
    p = await asyncio.create_subprocess_exec(*cmd_list)
    return await p.wait()


def create_processor(
    base_url: str,
    work_folder: str,
    drive: Drive,
    node: Node,
) -> VideoProcessor:
    table = {
        'video/mp4': MP4Processer,
        'video/x-matroska': MKVProcesser,
        'video/x-msvideo': MaybeH264Processer,
        'video/x-ms-wmv': NeverH264Processer,
        # '.mp4': MP4Processer,
        # '.mkv': MKVProcesser,
        # '.flv': MaybeH264Processer,
        # '.avi': MaybeH264Processer,
        # '.asf': MaybeH264Processer,
        # '.wmv': NeverH264Processer,
        # '.rm': NeverH264Processer,
        # '.rmvb': NeverH264Processer,
        # '.ogm': NeverH264Processer,
    }
    if node.mime_type not in table:
        return None
    constructor = table[node.mime_type]
    return constructor(base_url, work_folder, drive, node)
