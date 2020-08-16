import os
import typing
from argparse import ArgumentParser
import tarfile
import struct
import shutil
import io


def parse_params():
    parser = ArgumentParser(description='NXT3 SFX packer')
    parser.add_argument('--output-sh', action='store', required=True, metavar='FILE.SH',
                        help='Where to write the installer executable')
    parser.add_argument('--output-rbf', action='store', required=True, metavar='FILE.RBF',
                        help='Where to write the installer VM wrapper')
    parser.add_argument('--output-uf2', action='store', required=True, metavar='FILE.UF2',
                        help='Where to write the installer UF2 package')
    parser.add_argument('--sfx', action='store', required=True, metavar='SFX.SH',
                        help='Where to take the SFX shim from')
    parser.add_argument('--nxt3', action='store', required=True, metavar='FILE.ELF',
                        help='Where to take the NXT3 executable from')
    parser.add_argument('--battd', action='store', required=True, metavar='FILE.ELF',
                        help='Where to take the BattD executable from')
    parser.add_argument('--include-data', action='append', required=False, metavar='RESOURCE',
                        help='Include additional data files in the data folder')
    return parser.parse_args()


def main():
    info = parse_params()
    write_sfx(info.nxt3, info.battd, info.include_data, info.sfx, info.output_sh)
    write_rbf(info.output_rbf)
    write_uf2(info.output_uf2, info.output_sh, info.output_rbf)


def write_sfx(nxt3, battd, include_data, sfx, output):
    rbf_contents = mkrbf("/home/root/lms2012/prjs/NXT3/nxt3.elf")
    rbf = io.BytesIO(rbf_contents)
    tar_gz = io.BytesIO()
    with tarfile.open(fileobj=tar_gz, mode='w:gz') as archive:
        archive.add(name=nxt3, arcname="NXT3/nxt3.elf", filter=make_file(True))
        archive.add(name=battd, arcname="NXT3/battd.elf", filter=make_file(True))
        archive.addfile(make_rbf("NXT3/Run NXT3.rbf", rbf_contents), rbf)
        rbf.close()
        archive.addfile(make_directory("NXT3/data"))
        for path in include_data:
            archive.add(name=path,
                        arcname=f"NXT3/data/{os.path.basename(path)}",
                        filter=make_file(False))
    tar_gz.seek(0)
    with open(output, 'wb') as output:
        with open(sfx, 'rb') as sfx_extractor:
            shutil.copyfileobj(sfx_extractor, output)
            shutil.copyfileobj(tar_gz, output)
    tar_gz.close()


def write_rbf(path):
    with open(path, "wb") as fp:
        fp.write(mkrbf("../prjs/BrkProg_SAVE/NXT3_Installer.elf"))


def write_uf2(path, sh, rbf):
    with open(path, 'wb') as fp:
        fp.write(mkuf2({
            rbf: 'Projects/NXT3_Installer.rbf',
            sh: 'Projects/NXT3_Installer.elf'
        }))


def make_file(executable: bool):
    def file_filter(info: tarfile.TarInfo):
        info.mode = 0o00777 if executable else 0o00666
        info.mtime = 0
        info.type = tarfile.REGTYPE
        info.uid = info.gid = 0
        info.uname = info.gname = "root"
        info.pax_headers = {}
        return info

    return file_filter


def make_directory(path):
    info = tarfile.TarInfo(path)
    info.name = path
    info.size = 0
    info.mtime = 0
    info.mode = 0o00777
    info.type = tarfile.DIRTYPE
    info.uid = info.gid = 0
    info.uname = info.gname = "root"
    info.pax_headers = {}
    return info


def make_rbf(path, buffer):
    info = tarfile.TarInfo(path)
    info.size = len(buffer)
    return make_file(False)(info)


def mkrbf(cmd: str, end_time: int = 500):
    # rbf header
    result = bytearray()
    result += b'LEGO'
    length_offset = len(result)
    result += struct.pack('<i', 0)  # file length (fixup later)
    result += struct.pack('<h', 104)  # bytecode version
    result += struct.pack('<h', 1)  # object count
    result += struct.pack('<i', 0)  # global bytes
    # object header
    ip_offset = len(result)
    result += struct.pack('<i', 0)  # instruction offset (fixup later)
    result += struct.pack('<h', 0)  # owner object
    result += struct.pack('<h', 0)  # trigger count
    result += struct.pack('<i', 8)  # local bytes
    # object instructions
    ip = len(result)
    result += b'\x60\x80' + cmd.encode('ascii') + b'\0\x44'  # opSYSTEM(path, &v4)
    result += struct.pack('<BBhB', 0x85, 0x82, end_time, 0x40)  # opTIMER_WAIT(1000 ms, &v0)
    result += struct.pack('<BB', 0x86, 0x40)  # opTIMER_READY(v0)
    result += struct.pack('<B', 0x0A)  # opOBJECT_END
    length = len(result)

    struct.pack_into('<i', result, length_offset, length)
    struct.pack_into('<i', result, ip_offset, ip)
    return result


def mkuf2(paths: typing.Dict[str, str]):
    blocks = b''
    files = []
    blocks_now = 0
    blocks_total = 0

    for src, dest in paths.items():
        if len(dest) > 150:
            raise ValueError("UF2 filename is limited to 150 non-null bytes")
        size = os.path.getsize(src)
        payload = 476 - len(dest) - 1
        if payload > size:
            payload = size
        if payload > 466:
            payload = 466
        blocks_total += (size + payload - 1) // payload
        files.append({
            'src': src,
            'name': dest,
            'bytes': size,
            'payload': payload,
        })

    for file in files:
        name = file['name'].encode('ascii')
        size_now = 0
        size_total = file['bytes']
        payload = file['payload']
        with open(file['src'], "rb") as fp:
            buffer = fp.read(payload)
            while size_now < size_total:
                block = struct.pack(
                    '<IIIIIIII',
                    0x0A324655,    # UF2 header
                    0x9E5D5157,    # UF2 magic start
                    0x00001000,    # file container flag
                    size_now,      # target address
                    len(buffer),   # this payload
                    blocks_now,    # current block number
                    blocks_total,  # total block count
                    size_total,    # total size
                )
                block += buffer
                block += name
                block = block.ljust(508, b'\0')
                block += struct.pack('<I', 0x0AB16F30)  # UF2 magic end

                assert (len(block) == 512)
                blocks += block
                blocks_now += 1
                size_now += len(buffer)
                buffer = fp.read(payload)

    assert(blocks_now == blocks_total)
    return blocks


if __name__ == '__main__':
    main()
