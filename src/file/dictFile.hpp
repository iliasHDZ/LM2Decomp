#pragma once

#include "../common.hpp"
#include "../allocator.hpp"

#include "fileManager.hpp"

#include "parser.hpp"

class DictLoader;
class DictFile;

struct FileCollection {
    u8* m_datas[8];
    resource_node* m_mainDataFile;
    u32 m_entryCount;
    DictFile* m_dictFile;
    u32 m_sizes[8];
};

class DictFile {
public:
    static DictFile* Create(Allocator* alloc);

public:
    virtual void CreateResourceReader(ResourceReader* rd);

public:
    DictLoader* m_loader;
    Allocator* m_allocs[8];

    // [...]

    FileCollection m_files;
    u8  unk_80;
    u8  unk_81;
    u32* unk_84;
    u32* unk_88;
};

struct dict_file_table_entry {
    u32 m_offset;
    u32 m_decompressedSize;
    u32 m_compressedSize;
    u16 unk_0c;
    u8  m_otherFileIndex;
    u8  unk_0f;
};

struct dict_file_group {
    u16 m_dataEntryCount;
    u16 m_mainFileIndex;
};

struct dict_file_primary_header {
    u32 m_totalFileCount;
    u32 unk_04;
    u16 m_groupsPerDir;
    u8  m_dirCount;
    u8  m_externalFilesCount;
};

struct dict_file_dir_entry {
    u32 unk_00;
    u8  m_indicies[8];
};

struct dict_file_header {
    u32 m_magic;
    u16 unk_04;
    u8  unk_06;
    dict_file_primary_header m_primary;
    dict_file_dir_entry m_dirEntries[0];
};

typedef void(*FilesLoadedCallback)(DictFile*, void*);

typedef void(*DictLoadedCallback)(DictLoader*, void*);

class DictLoader {
public:
    DictLoader();

    DictFile* LoadDictAsync(char* path, Allocator* alloc, FilesLoadedCallback callback, void* param, u32 secIdx, u8 unk);

    DictFile* LoadDictSync(char* path, Allocator* alloc, u32 secIdx);

    DictFile* LoadDict(char* path, Allocator* alloc, FilesLoadedCallback callback, void* param, FileManager* fman, u32 secIdx, u8 unk);

    void LoadDictFile(char* path, Allocator* alloc, DictLoadedCallback callback, void* param);

    static void ReadDictFileCallback(File*, void*, u32, void* loader);

    void ParseDictFile();

    static void DictFileLoadedCallback(DictLoader* loader, void*);

    inline u8* AllocFileBuffer(u32 fileIdx, u32 idx);

    inline void ReadFile(u32 fileIdx, u32 idx, u8* buffer);

    void LoadAllFiles();

    static void ReadFileCallback(File*, void*, u32, void* loader);

    static void ReadCompressedFileCallback(File*, void*, u32, void* loader);

    void LoadFileRecursive();

    void CleanUp();

    static void FinishCallback(DictLoader*, void*);

public:
    char* m_path;
    u32 m_pathStringLength;
    File* m_file;
    File* m_otherFiles[4];
    char* m_otherFileExtensions[4];
    dict_file_header* m_dictBuffer;
    dict_file_primary_header* m_primaryHeader;
    dict_file_dir_entry* m_dirEntries;
    dict_file_group* m_fileGroups;
    dict_file_table_entry* m_fileTable;
    u8 m_state;
    int m_dirIndex;
    int m_fileSectionIndex;
    int m_currentIndex;
    DictFile* m_dictFile;
    DictLoadedCallback m_onDictLoaded;
    FilesLoadedCallback m_onFilesLoaded;
    void* m_callbackParam;
    Allocator* m_alloc;
    FileManager* m_fileMan;
    u32 m_specialFilesBitmap;
    u8* m_decompressedBuffer;
    u8* m_compressedBuffer;
    dict_file_table_entry* m_currentFileEntry;
    u32 unk_78;
    u8  unk_7c;
};