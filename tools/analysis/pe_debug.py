# The debug information a Windows module (PE/COFF image) points to, read from the image itself (Microsoft PE Format:
# the optional header's Debug data directory, its IMAGE_DEBUG_DIRECTORY entries, a CodeView entry's RSDS record): the
# program database (.pdb) a module names, and whether it can be found beside the module or where the record says. A
# module whose program database is nowhere has no debug information here, whatever a symbolizer would say of it.
from __future__ import annotations

import os
import struct

PE_POINTER = 0x3C                 # e_lfanew: where the PE signature is
COFF_SIZE = 24                    # the signature (4) and the COFF file header (20)
SECTIONS_AT, OPTIONAL_SIZE_AT = 6, 20  # in the COFF header: NumberOfSections, SizeOfOptionalHeader
MAGIC_PE32_PLUS = 0x20B
DIRECTORIES_PE32, DIRECTORIES_PE32_PLUS = 96, 112      # where the data directories start in the optional header
DEBUG_DIRECTORY = 6               # the Debug data directory's index
DIRECTORY_SIZE, SECTION_SIZE, DEBUG_ENTRY_SIZE = 8, 40, 28
SECTION_FIELDS = 8                # a section header's VirtualSize, after its 8-byte name
DEBUG_FIELDS = 12                 # a debug entry's Type, after Characteristics, TimeDateStamp and the versions
CODEVIEW = 2                      # IMAGE_DEBUG_TYPE_CODEVIEW
RSDS = b"RSDS"
RSDS_PATH = 24                    # the signature (4), the GUID (16) and the age (4) come before the path


def database(path: str) -> str | None:
    """The program database a module's CodeView record names, None where it names none."""
    with open(path, "rb") as f:
        data = f.read()
    pe = struct.unpack_from("<I", data, PE_POINTER)[0]
    sections = struct.unpack_from("<H", data, pe + SECTIONS_AT)[0]
    optional_size = struct.unpack_from("<H", data, pe + OPTIONAL_SIZE_AT)[0]
    optional = pe + COFF_SIZE
    magic = struct.unpack_from("<H", data, optional)[0]
    directories = optional + (DIRECTORIES_PE32_PLUS if magic == MAGIC_PE32_PLUS else DIRECTORIES_PE32)
    rva, size = struct.unpack_from("<II", data, directories + DEBUG_DIRECTORY * DIRECTORY_SIZE)
    table = file_offset(data, optional + optional_size, sections, rva)
    if not size or table is None:
        return None
    for entry in range(table, table + size, DEBUG_ENTRY_SIZE):
        kind, length, _, raw = struct.unpack_from("<IIII", data, entry + DEBUG_FIELDS)
        if kind == CODEVIEW and data[raw:raw + len(RSDS)] == RSDS:
            return data[raw + RSDS_PATH:raw + length].split(b"\0", 1)[0].decode("utf-8", "replace")
    return None


def file_offset(data: bytes, section_table: int, sections: int, rva: int) -> int | None:
    """Where in the file an address relative to the image base is: inside the section that holds it."""
    for i in range(sections):
        size, address, raw_size, raw = struct.unpack_from("<IIII", data, section_table + i * SECTION_SIZE + SECTION_FIELDS)
        if address <= rva < address + max(size, raw_size):
            return rva - address + raw
    return None


def database_missing(path: str) -> bool:
    """Whether a module names a program database that is neither where its record says nor beside the module."""
    named = database(path)
    if named is None:
        return False
    beside = os.path.join(os.path.dirname(path), os.path.basename(named.replace("\\", "/")))
    return not os.path.exists(named) and not os.path.exists(beside)
