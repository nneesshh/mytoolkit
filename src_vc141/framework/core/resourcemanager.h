#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <deque>
#include "declarations.h"

// @bindsingleton g_resources
class ResourceManager {
public:
    // @dontbind
    void init(const char *argv0);
    // @dontbind
    void terminate();

    bool discoverWorkDir(const std::string& existentFile);
    bool setupUserWriteDir(const std::string& appWriteDirName);
    bool setWriteDir(const std::string& writeDir, bool create = false);

    bool addSearchPath(const std::string& path, bool pushFront = false);
    bool removeSearchPath(const std::string& path);
    void searchAndAddPackages(const std::string& packagesDir, const std::string& packagesExt);

    bool fileExists(const std::string& fileName);
    bool directoryExists(const std::string& directoryName);

    // @dontbind
    void readFileStream(const std::string& fileName, std::iostream& out);
    std::string readFileContents(const std::string& fileName);
    // @dontbind
    bool writeFileBuffer(const std::string& fileName, const uchar* data, uint size);
    bool writeFileContents(const std::string& fileName, const std::string& data);
    // @dontbind
    bool writeFileStream(const std::string& fileName, std::iostream& in);

    FileStreamPtr openFile(const std::string& fileName);
    FileStreamPtr appendFile(const std::string& fileName);
    FileStreamPtr createFile(const std::string& fileName);
    bool deleteFile(const std::string& fileName);

    bool makeDir(const std::string directory);
    std::list<std::string> listDirectoryFiles(const std::string& directoryPath = "");

    std::string resolvePath(const std::string& path);
    std::string getRealDir(const std::string& path);
    std::string getRealPath(const std::string& path);
    std::string getBaseDir();
    std::string getUserDir();
    std::string getWriteDir() { return m_writeDir; }
    std::string getWorkDir() { return m_workDir; }
    std::deque<std::string> getSearchPaths() { return m_searchPaths; }

    std::string guessFilePath(const std::string& filename, const std::string& type);
    bool isFileType(const std::string& filename, const std::string& type);
    ticks_t getFileTime(const std::string& filename);

private:
    std::string m_workDir;
    std::string m_writeDir;
    std::deque<std::string> m_searchPaths;
};

extern ResourceManager g_resources;

#endif
