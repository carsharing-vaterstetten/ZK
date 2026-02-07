#pragma once
#include <FS.h>
#include <WString.h>

struct FileInfo
{
    String path;
    size_t size;
};

class SwappableFile : public Print
{
public:
    SwappableFile(const String& fileAPath, const String& fileBPath) : fileAPath(fileAPath), fileBPath(fileBPath) {}

    bool begin(bool moveBOverToA = true);
    void end();
    bool swapToA();
    bool appendBToA(bool deleteBAfterwards);
    bool appendBToAAndSwapToA(bool deleteBAfterwards = true);
    bool replaceAwithBAndSwapToA();
    bool swapToB();
    bool swap();
    [[nodiscard]] std::optional<File> getCurrentFile() const;
    [[nodiscard]] std::optional<FileInfo> getCurrentFileInfo() const;
    void flush() override;
    void close();

protected:
    String fileAPath, fileBPath;
    std::optional<File> currentLogFile;
    size_t write(const uint8_t* buffer, size_t size) override;
    size_t write(uint8_t b) override;
    bool isA = true;
    bool swapToPath(const String& path);
};
