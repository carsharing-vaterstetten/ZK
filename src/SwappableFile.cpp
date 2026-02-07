#include "SwappableFile.h"

#include <LittleFS.h>

#include "Globals.h"
#include "StorageManager.h"

bool SwappableFile::begin(const bool moveBOverToA)
{
    if (moveBOverToA)
        return appendBToAAndSwapToA();

    return swapToA();
}

void SwappableFile::end()
{
    close();
}

size_t SwappableFile::write(const uint8_t* buffer, const size_t size)
{
    if (!currentLogFile)
        return 0;
    return currentLogFile->write(buffer, size);
}

size_t SwappableFile::write(const uint8_t b)
{
    if (!currentLogFile)
        return 0;
    return currentLogFile->write(b);
}

bool SwappableFile::swapToPath(const String& path)
{
    if (path != fileAPath && path != fileBPath) return false;

    if (currentLogFile)
        currentLogFile->close();

    const File file = LittleFS.open(path, FILE_APPEND, true);

    if (!file)
    {
        fileLog.errorln("Failed to open " + path);
        return false;
    }

    currentLogFile.emplace(file);

    isA = path == fileAPath;
    fileLog.infoln("Swapped to " + String(currentLogFile->path()));

    return true;
}

bool SwappableFile::swapToA()
{
    return swapToPath(fileAPath);
}

bool SwappableFile::appendBToA(const bool deleteBAfterwards)
{
    if (currentLogFile)
        currentLogFile->close();

    if (!LittleFS.exists(fileBPath))
    {
        serialOnlyLog.infoln(fileBPath + " doesn't exist. Nothing to append to " + fileAPath);
        return true;
    }

    File fileB = LittleFS.open(fileBPath, FILE_READ);

    if (!fileB) return false;
    if (fileB.size() <= 0)
    {
        serialOnlyLog.infoln(fileBPath + " is empty. Nothing to append to " + fileAPath);
        fileB.close();
        if (deleteBAfterwards)
            serialOnlyLog.logInfoOrErrorln(LittleFS.remove(fileBPath), "Deleted " + fileBPath,
                                           "Failed to delete " + fileBPath);
        return true;
    }

    File fileA = LittleFS.open(fileAPath, FILE_APPEND, true);

    if (!fileA)
    {
        fileB.close();
        return false;
    }

    serialOnlyLog.debugln("Appending " + String(fileB.size()) + " B from " + fileB.path() + " to " + fileA.path());

    constexpr size_t bufSize = 512;
    uint8_t buf[bufSize];

    while (fileB.available())
    {
        const size_t read = fileB.read(buf, bufSize);
        fileA.write(buf, read);
    }

    fileA.close();
    fileB.close();

    if (deleteBAfterwards)
        serialOnlyLog.logInfoOrErrorln(LittleFS.remove(fileBPath), "Deleted " + fileBPath,
                                       "Failed to delete " + fileBPath);

    return true;
}

/// Make sure both files are closed before calling this function.
/// Use replaceAwithBAndSwapToA if A got deleted / doesn't exist / is empty
bool SwappableFile::appendBToAAndSwapToA(const bool deleteBAfterwards)
{
    const bool appendSuccess = appendBToA(deleteBAfterwards);
    swapToA();
    return appendSuccess;
}

bool SwappableFile::replaceAwithBAndSwapToA()
{
    if (currentLogFile)
        currentLogFile->close();
    StorageManager::move(fileBPath, fileAPath, true);
    return swapToA();
}

bool SwappableFile::swapToB()
{
    return swapToPath(fileBPath);
}

bool SwappableFile::swap()
{
    if (isA)
        return swapToB();

    return swapToA();
}

/// Use getCurrentFileInfo over getCurrentFile if possible
std::optional<File> SwappableFile::getCurrentFile() const
{
    return currentLogFile;
}

/// Use getCurrentFileInfo over getCurrentFile if possible
std::optional<FileInfo> SwappableFile::getCurrentFileInfo() const
{
    if (!currentLogFile) return std::nullopt;

    return FileInfo{
        .path = currentLogFile->path(),
        .size = currentLogFile->size(),
    };
}

void SwappableFile::flush()
{
    if (currentLogFile)
        currentLogFile->flush();
}

void SwappableFile::close()
{
    if (currentLogFile)
        currentLogFile->close();
}
