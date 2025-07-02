#include "dictFile.hpp"

#include <string.h>

#include "../ext/zlib/zlib.h"
#include "../ext/zlib/inflate.h"

DictFile* DictFile::Create(Allocator* alloc) {
    DictFile* ret = new DictFile();

    for (int i = 0; i < 8; i++)
        ret->m_allocs[i] = alloc;
    return ret;
}

void DictFile::CreateResourceReader(ResourceReader* rd) {
    rd->m_files = &m_files;
    rd->m_nodeTable = (resource_node*)m_files.m_mainDataFile;
    rd->m_count = m_files.m_entryCount;
    rd->m_index = 0;
}

DictLoader::DictLoader()
    : m_fileMan(nullptr) {}

DictFile* DictLoader::LoadDictAsync(char* path, Allocator* alloc, FilesLoadedCallback callback, void* param, u32 secIdx, u8 unk) {
    return LoadDict(path, alloc, callback, param, &FileManager::s_instance, secIdx, unk);
}

DictFile* DictLoader::LoadDictSync(char* path, Allocator* alloc, u32 secIdx) {
    return LoadDict(path, alloc, nullptr, nullptr, &FileManager::s_otherInstance, secIdx, 0);
}

DictFile* DictLoader::LoadDict(char* path, Allocator* alloc, FilesLoadedCallback callback, void* param, FileManager* fman, u32 secIdx, u8 unk) {
    DictFile* dict = DictFile::Create(alloc);

    DictLoader* loader = new DictLoader();

    loader->m_dictFile = dict;
    loader->unk_7c = unk;
    dict->m_loader = loader;
    loader->m_onFilesLoaded = callback;
    loader->m_fileSectionIndex = secIdx;
    loader->m_fileMan = fman;

    loader->LoadDictFile(path, alloc, DictFileLoadedCallback, param);
    return dict;
}

void DictLoader::LoadDictFile(char* path, Allocator* alloc, DictLoadedCallback callback, void* param) {
    m_onDictLoaded = callback;
    m_callbackParam = param;

    m_alloc = alloc;

    m_pathStringLength = strlen(path);

    m_path = (char*)alloc->malloc(m_pathStringLength + 11);
    
    memcpy(m_path, path, m_pathStringLength);
    memcpy(m_path + m_pathStringLength, ".dict", 5);
    m_path[m_pathStringLength + 5] = 0;

    File* file = m_fileMan->Open(m_path);
    m_file = file;

    if (file == nullptr) return;

    m_otherFiles[0] = nullptr;
    m_otherFiles[1] = nullptr;
    m_otherFiles[2] = nullptr;
    m_otherFiles[3] = nullptr;

    u32 size = m_fileMan->GetSize(file);
    u8* buffer = (u8*)alloc->malloc(size);

    m_dictBuffer = (dict_file_header*)buffer;
    m_state = 1;

    m_fileMan->Read(file, buffer, size, DictLoader::ReadDictFileCallback, this);
}

void DictLoader::ReadDictFileCallback(File*, void*, u32, void* loader) {
    ((DictLoader*)loader)->ParseDictFile();
}

void DictLoader::ParseDictFile() {
    m_fileMan->Close(m_file);
    m_file = nullptr;

    dict_file_header* header = (dict_file_header*)m_dictBuffer;

    m_primaryHeader = &header->m_primary;
    m_dirEntries    = header->m_dirEntries;

    if (m_fileSectionIndex == -1)
        m_fileSectionIndex = 0;
    
    u8* ptr = (u8*)header->m_dirEntries;

    u32 dirCount = m_primaryHeader->m_dirCount;

    ptr += dirCount * sizeof(dict_file_dir_entry);
    m_fileGroups = (dict_file_group*)ptr;

    u32 groupsPerDir = m_primaryHeader->m_groupsPerDir;

    m_fileTable = (dict_file_table_entry*)(ptr + dirCount * groupsPerDir * sizeof(dict_file_group));

    char* fext = (char*)(m_fileTable + m_primaryHeader->m_totalFileCount);

    for (u32 i = 0; i < m_primaryHeader->m_externalFilesCount; i++) {
        m_otherFileExtensions[i] = fext;
        fext += strlen(fext) + 1;
    }

    if (header->unk_06) {
        // Not sure about this. It doesn't use the usual malloc. Might be something equivalent to new[] instead.
        m_compressedBuffer = (u8*)m_alloc->malloc(m_primaryHeader->unk_04);
    }

    m_state = 2;
    
    if (m_onDictLoaded != nullptr)
        m_onDictLoaded(this, m_callbackParam);
}

void DictLoader::DictFileLoadedCallback(DictLoader* loader, void*) {
    loader->m_dirIndex = 0;
    loader->m_onDictLoaded = FinishCallback; // TODO

    loader->LoadAllFiles();
}

u8* DictLoader::AllocFileBuffer(u32 fileIdx, u32 idx) {
    u32 unk0 = m_dictBuffer->unk_06 ? 8 : 4;
    u32 unk1 = unk0 > m_fileTable[fileIdx].unk_0c ? unk0 : m_fileTable[fileIdx].unk_0c;

    u8* buffer;
    if (m_fileTable[fileIdx].unk_0f) {
        m_specialFilesBitmap |= 1 << (idx & 0xff);
        // This malloc uses a different function
        buffer = (u8*)m_dictFile->m_allocs[idx]->malloc(m_fileTable[fileIdx].m_decompressedSize);
    } else {
        // This malloc uses a different function, it also accepts unk1 as a secondary parameter for some reason
        buffer = (u8*)m_dictFile->m_allocs[idx]->malloc(m_fileTable[fileIdx].m_decompressedSize);
    }

    return buffer;
}

void DictLoader::ReadFile(u32 fileIdx, u32 idx, u8* buffer) {
    dict_file_table_entry* fileEntry = &m_fileTable[fileIdx];
    if (m_otherFiles[fileEntry->m_otherFileIndex] == nullptr) {
        char* ext = m_otherFileExtensions[fileEntry->m_otherFileIndex];

        int i = 0;
        for (; ext[i]; i++)
            m_path[m_pathStringLength + i] = ext[i];
        m_path[m_pathStringLength + i] = 0;

        m_otherFiles[fileEntry->m_otherFileIndex] = m_fileMan->Open(m_path);
    }

    m_fileMan->Seek(m_otherFiles[fileEntry->m_otherFileIndex], fileEntry->m_offset);

    if (fileEntry->m_compressedSize == 0) {
        m_fileMan->Read(m_otherFiles[fileEntry->m_otherFileIndex], buffer, m_fileTable[fileIdx].m_decompressedSize, ReadFileCallback, this);
    } else {
        m_currentFileEntry = fileEntry;
        m_decompressedBuffer = buffer;
        m_fileMan->Read(m_otherFiles[fileEntry->m_otherFileIndex], m_compressedBuffer, m_fileTable[fileIdx].m_compressedSize, ReadCompressedFileCallback, this);
    }
}

void DictLoader::LoadAllFiles() {
    m_state = 3;
    m_currentIndex = 0;
    m_specialFilesBitmap = 0;

    // [...]

    dict_file_group* group = &m_fileGroups[m_fileSectionIndex + m_dirIndex * m_primaryHeader->m_dirCount];

    m_dictFile->m_files.m_entryCount = group->m_dataEntryCount;

    u32 mainIdx = group->m_mainFileIndex;

    u8* buffer = AllocFileBuffer(mainIdx, 0);

    m_dictFile->m_files.m_mainDataFile = (resource_node*)buffer;

    m_dictFile->m_files.m_sizes[0] = 0;
    m_dictFile->m_files.m_sizes[1] = 0;
    m_dictFile->m_files.m_sizes[2] = 0;
    m_dictFile->m_files.m_sizes[3] = 0;
    m_dictFile->m_files.m_sizes[4] = 0;
    m_dictFile->m_files.m_sizes[5] = 0;
    m_dictFile->m_files.m_sizes[6] = 0;
    m_dictFile->m_files.m_sizes[7] = 0;

    ReadFile(mainIdx, 0, buffer);
}

void DictLoader::ReadFileCallback(File*, void*, u32, void* loader) {
    ((DictLoader*)loader)->LoadFileRecursive();
}

void* InflateAlloc(void* alloc, u32 items, u32 size) {
    return ((Allocator*)alloc)->malloc(items * size);
}

void InflateFree(void* alloc, void* address) {
    ((Allocator*)alloc)->free(address);
}

void DecompressBuffer(Allocator* alloc, u8* src, u32 src_size, u8* dst, u32 dst_size) {
    int err;
    z_stream stream;

    stream.zalloc = InflateAlloc;
    stream.avail_in = 0;
    stream.zfree = InflateFree;
    stream.next_in = 0;
    stream.opaque = alloc;
    inflateInit(&stream);

    while (!err) {
        stream.avail_in = src_size > 0x40000 ? 0x40000 : src_size;
        if (stream.avail_in == 0) break;

        u8* next_src = src + stream.avail_in;
        src_size -= stream.avail_in;
        stream.next_in = src;

        while (stream.avail_out == 0) {
            stream.avail_out = 0x40000;
            stream.next_out  = dst;
            err = inflate(&stream, Z_NO_FLUSH);
            dst += 0x40000 - stream.avail_out;
        }

        src = next_src;
    }
    inflateEnd(&stream);
}

void DictLoader::ReadCompressedFileCallback(File*, void* buffer, u32 size, void* ldr) {
    DictLoader* loader = (DictLoader*)ldr;

    if (loader->unk_7c == 0) {
        DecompressBuffer(loader->m_alloc, (u8*)buffer, size, loader->m_decompressedBuffer, loader->m_currentFileEntry->m_decompressedSize);
        loader->LoadFileRecursive();
        return;
    }

    // [...]
}

void DictLoader::LoadFileRecursive() {
    u32 dirCount = m_primaryHeader->m_dirCount;
    u32 idx = m_currentIndex;

    while (true) {
        if (idx > 7) {
            m_dictFile->unk_81 = m_specialFilesBitmap;
            m_dictFile->unk_80 = 1;
            m_state = 2;

            if (m_onDictLoaded) {
                m_onDictLoaded(this, m_callbackParam);
                return;
            }

            if (m_onFilesLoaded) {
                m_onFilesLoaded(m_dictFile, m_callbackParam);
                return;
            }

            return;
        }

        u32 fileIdx = m_dirEntries[m_fileSectionIndex].m_indicies[idx];
        if (fileIdx) {
            fileIdx += m_fileGroups[m_fileSectionIndex + m_dirIndex * dirCount].m_mainFileIndex;
            if (m_fileTable[fileIdx].m_decompressedSize != 0) {
                m_currentIndex = idx + 1;

                u8* buffer = AllocFileBuffer(fileIdx, idx);

                m_dictFile->m_files.m_datas[idx] = buffer;
                m_dictFile->m_files.m_sizes[idx] = m_fileTable[fileIdx].m_decompressedSize;

                ReadFile(fileIdx, idx, buffer);
                return;
            }
        }
    }
}

void DictLoader::CleanUp() {
    if (m_state == 0) return;

    for (u32 i = 0; i < 4; i++) {
        if (m_otherFiles[i]) {
            m_fileMan->Close(m_otherFiles[i]);
            m_otherFiles[i] = nullptr;
        }
    }

    if (m_state == 1) {
        m_fileMan->Close(m_file);
        m_file = nullptr;
    }

    m_alloc->free(m_dictBuffer);
    if (m_compressedBuffer) {
        m_alloc->free(m_compressedBuffer);
        m_compressedBuffer = nullptr;
    }

    m_alloc->free(m_path);
    m_path = nullptr;

    if (unk_78 != 0) {
        // [...] Some inflateEnd is applied to it.
        m_alloc->free((void*)unk_78);
        unk_78 = 0;
    }

    m_state = 0;
}

void DictLoader::FinishCallback(DictLoader* loader, void* param) {
    if (loader->m_onFilesLoaded)
        loader->m_onFilesLoaded(loader->m_dictFile, param);
    
    loader->m_dictFile->m_loader = nullptr;
    Allocator* alloc = loader->m_dictFile->m_allocs[0];

    loader->CleanUp();
    alloc->free(loader);
}