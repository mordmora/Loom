#ifndef IO_H
#define IO_H

#include <cstdint>
#include <string>
#include <vector>
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else 
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif
#include <iostream>
namespace loom {

    struct SourceFileView {
        const uint8_t* buff;
        std::size_t    size;
        uint32_t       global_start;
        uint16_t       id;
    };

    class SourceManager {
    public:
        SourceManager() = default;
        SourceManager(const SourceManager&) = delete;
        SourceManager& operator=(const SourceManager&) = delete;

        ~SourceManager() {
            for (std::size_t i = 0; i < buffs.size(); i++) {
                #ifdef _WIN32
                if (buffs[i])              UnmapViewOfFile(buffs[i]);
                if (hMaps[i])              CloseHandle(hMaps[i]);
                if (hFiles[i] != INVALID_HANDLE_VALUE) CloseHandle(hFiles[i]);
                #else
                if (buffs[i] && buffs[i] != reinterpret_cast<const uint8_t*>(MAP_FAILED))
                    munmap((void*)buffs[i], sizes[i]);
                if (fds[i] != -1)
                    close(fds[i]);
                #endif
            }
        }

        uint16_t loadFile(const std::string& path) {
            uint16_t id = static_cast<uint16_t>(buffs.size());

            const uint8_t* buff = nullptr;
            std::size_t    size = 0;

            #ifdef _WIN32
            HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                                       NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE) {
                std::cerr << "[Error] No se pudo abrir: " << path << "\n";
                return 0xFFFF;
            }
            LARGE_INTEGER fs;
            GetFileSizeEx(hFile, &fs);
            size = static_cast<std::size_t>(fs.QuadPart);
            HANDLE hMap = NULL;
            if (size > 0) {
                hMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
                if (hMap)
                    buff = static_cast<const uint8_t*>(MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0));
            }
            hFiles.push_back(hFile);
            hMaps.push_back(hMap);
            #else
            int fd = open(path.c_str(), O_RDONLY);
            if (fd == -1) {
                std::cerr << "[Error] no se pudo abrir " << path << "\n";
                return 0xFFFF;
            }
            struct stat st;
            if (fstat(fd, &st) == 0) {
                size = static_cast<std::size_t>(st.st_size);
                if (size > 0) {
                    void* m = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
                    if (m != MAP_FAILED)
                        buff = reinterpret_cast<const uint8_t*>(m);
                }
            }
            fds.push_back(fd);
            #endif

            if (!buff && size > 0) {
                std::cerr << "[Error] falló el memory map de " << path << "\n";
                return 0xFFFF;
            }

            buffs.push_back(buff);
            sizes.push_back(size);
            global_starts.push_back(next_glob_offset);
            next_glob_offset += static_cast<uint32_t>(size);

            std::vector<uint32_t> offsets;
            offsets.push_back(0);
            for (std::size_t i = 0; i < size; i++)
                if (buff[i] == '\n') offsets.push_back(static_cast<uint32_t>(i + 1));
            line_offsets.push_back(std::move(offsets));

            paths.push_back(path);

            return id;
        }

        SourceFileView getView(uint16_t i) const {
            return { buffs[i], sizes[i], global_starts[i], i };
        }

        const std::vector<uint32_t>& getLineOffsets(uint16_t i) const {
            return line_offsets[i];
        }

        const std::string& getPath(uint16_t i) const {
            return paths[i];
        }

        std::size_t fileCount() const { return buffs.size(); }

    private:
        std::vector<const uint8_t*> buffs;
        std::vector<std::size_t>    sizes;
        std::vector<uint32_t>       global_starts;

        std::vector<std::vector<uint32_t>> line_offsets;

        std::vector<std::string> paths;
        #ifdef _WIN32
        std::vector<HANDLE> hFiles;
        std::vector<HANDLE> hMaps;
        #else
        std::vector<int> fds;
        #endif

        uint32_t next_glob_offset = 0;
    };

}    



#endif
