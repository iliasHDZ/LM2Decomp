#pragma once

#include "../common.hpp"

struct File {
    // [...]
};

typedef void (*FileAccessCallback)(File*, void*, u32, void*);

class FileManager {
public:
    virtual File* Open(char* path);

    virtual void Close(File* file);

    virtual void Read(File* file, void* buffer, u32 size, FileAccessCallback, void* param);

    virtual void Seek(File* file, u32 pos);

    virtual u32 GetSize(File* file, u32* size_ret = nullptr);

public:
    static FileManager s_instance;
    static FileManager s_otherInstance;
};