#include "resourcemanager.h"
#include "filestream.h"

#include "../physfs/physfs.h"
#include "cocos2d.h"

ResourceManager g_resources;

void ResourceManager::init(const char *argv0) {
    PHYSFS_init(argv0);
    PHYSFS_permitSymbolicLinks(1);
}

void ResourceManager::terminate() {
    PHYSFS_deinit();
}

bool ResourceManager::discoverWorkDir(const std::string& existentFile) {
    // search for modules directory
    std::string possiblePaths[] = { g_resources.getBaseDir(),
                                    g_resources.getBaseDir() + "../",
                                    g_resources.getBaseDir() + "../share/" };

    bool found = false;
    for(const std::string& dir : possiblePaths) {
        if(!PHYSFS_addToSearchPath(dir.c_str(), 0))
            continue;

        if(PHYSFS_exists(existentFile.c_str())) {
			CCLOGINFO("Found work dir at '%s'", dir);
            m_workDir = dir;
            found = true;
            break;
        }
        PHYSFS_removeFromSearchPath(dir.c_str());
    }

    return found;
}

bool ResourceManager::setupUserWriteDir(const std::string& appWriteDirName) {
    std::string userDir = getUserDir();
    std::string dirName;
#ifndef WIN32
    dirName = stdext::format(".%s", appWriteDirName);
#else
    dirName = appWriteDirName;
#endif
    std::string writeDir = userDir + dirName;

    if(!PHYSFS_setWriteDir(writeDir.c_str())) {
        if(!PHYSFS_setWriteDir(userDir.c_str()) || !PHYSFS_mkdir(dirName.c_str())) {
			CCLOGERROR("Unable to create write directory '%s': %s", writeDir, PHYSFS_getLastError());
            return false;
        }
    }
    return setWriteDir(writeDir);
}

bool ResourceManager::setWriteDir(const std::string& writeDir, bool create) {
    if(!PHYSFS_setWriteDir(writeDir.c_str())) {
		CCLOGERROR("Unable to set write directory '%s': %s", writeDir, PHYSFS_getLastError());
        return false;
    }

    if(!m_writeDir.empty())
        removeSearchPath(m_writeDir);

    m_writeDir = writeDir;

    if(!addSearchPath(writeDir))
		CCLOGERROR("Unable to add write '%s' directory to search path", writeDir);

    return true;
}

bool ResourceManager::addSearchPath(const std::string& path, bool pushFront) {
    std::string savePath = path;
    if(!PHYSFS_addToSearchPath(path.c_str(), pushFront ? 0 : 1)) {
        bool found = false;
        for(std::string searchPath : m_searchPaths) {
            std::string newPath = searchPath + path;
            if(PHYSFS_addToSearchPath(newPath.c_str(), pushFront ? 0 : 1)) {
                savePath = newPath;
                found = true;
                break;
            }
        }

        if(!found) {
            //CCLOGERROR("Could not add '%s' to directory search path. Reason %s", path, PHYSFS_getLastError());
            return false;
        }
    }
    if(pushFront)
        m_searchPaths.push_front(savePath);
    else
        m_searchPaths.push_back(savePath);
    return true;
}

bool ResourceManager::removeSearchPath(const std::string& path) {
    if(!PHYSFS_removeFromSearchPath(path.c_str()))
        return false;
    auto it = std::find(m_searchPaths.begin(), m_searchPaths.end(), path);
    assert(it != m_searchPaths.end());
    m_searchPaths.erase(it);
    return true;
}

void ResourceManager::searchAndAddPackages(const std::string& packagesDir, const std::string& packageExt) {
    auto files = listDirectoryFiles(packagesDir);
    for(auto it = files.rbegin(); it != files.rend(); ++it) {
        const std::string& file = *it;
        if(!stdext::ends_with(file, packageExt))
            continue;
        std::string package = getRealDir(packagesDir) + "/" + file;
        if(!addSearchPath(package, true))
			CCLOGERROR("Unable to read package '%s': %s", package, PHYSFS_getLastError());
    }
}

bool ResourceManager::fileExists(const std::string& fileName) {
    return (PHYSFS_exists(resolvePath(fileName).c_str()) && !PHYSFS_isDirectory(resolvePath(fileName).c_str()));
}

bool ResourceManager::directoryExists(const std::string& directoryName) {
    return (PHYSFS_isDirectory(resolvePath(directoryName).c_str()));
}

void ResourceManager::readFileStream(const std::string& fileName, std::iostream& out) {
    std::string buffer = readFileContents(fileName);
    if(buffer.length() == 0) {
        out.clear(std::ios::eofbit);
        return;
    }
    out.clear(std::ios::goodbit);
    out.write(&buffer[0], buffer.length());
    out.seekg(0, std::ios::beg);
}

std::string ResourceManager::readFileContents(const std::string& fileName) {
    std::string fullPath = resolvePath(fileName);

    PHYSFS_File* file = PHYSFS_openRead(fullPath.c_str());
    if(!file)
        stdext::throw_exception(stdext::format("unable to open file '%s': %s", fullPath, PHYSFS_getLastError()));

    int fileSize = PHYSFS_fileLength(file);
    std::string buffer(fileSize, 0);
    PHYSFS_read(file, (void*)&buffer[0], 1, fileSize);
    PHYSFS_close(file);

    return buffer;
}

bool ResourceManager::writeFileBuffer(const std::string& fileName, const uchar* data, uint size) {
    PHYSFS_file* file = PHYSFS_openWrite(fileName.c_str());
    if(!file) {
		CCLOGERROR(PHYSFS_getLastError());
        return false;
    }

    PHYSFS_write(file, (void*)data, size, 1);
    PHYSFS_close(file);
    return true;
}

bool ResourceManager::writeFileStream(const std::string& fileName, std::iostream& in) {
    std::streampos oldPos = in.tellg();
    in.seekg(0, std::ios::end);
    std::streampos size = in.tellg();
    in.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    in.read(&buffer[0], size);
    bool ret = writeFileBuffer(fileName, (const uchar*)&buffer[0], size);
    in.seekg(oldPos, std::ios::beg);
    return ret;
}

bool ResourceManager::writeFileContents(const std::string& fileName, const std::string& data) {
    return writeFileBuffer(fileName, (const uchar*)data.c_str(), data.size());
}

FileStreamPtr ResourceManager::openFile(const std::string& fileName) {
    std::string fullPath = resolvePath(fileName);

    PHYSFS_File* file = PHYSFS_openRead(fullPath.c_str());
    if(!file)
        stdext::throw_exception(stdext::format("unable to open file '%s': %s", fullPath, PHYSFS_getLastError()));
    return new FileStream(fullPath, file, false);
}

FileStreamPtr ResourceManager::appendFile(const std::string& fileName) {
    PHYSFS_File* file = PHYSFS_openAppend(fileName.c_str());
    if(!file)
        stdext::throw_exception(stdext::format("failed to append file '%s': %s", fileName, PHYSFS_getLastError()));
    return new FileStream(fileName, file, true);
}

FileStreamPtr ResourceManager::createFile(const std::string& fileName) {
    PHYSFS_File* file = PHYSFS_openWrite(fileName.c_str());
    if(!file)
        stdext::throw_exception(stdext::format("failed to create file '%s': %s", fileName, PHYSFS_getLastError()));
    return new FileStream(fileName, file, true);
}

bool ResourceManager::deleteFile(const std::string& fileName) {
    return PHYSFS_delete(resolvePath(fileName).c_str()) != 0;
}

bool ResourceManager::makeDir(const std::string directory) {
    return PHYSFS_mkdir(directory.c_str());
}

std::list<std::string> ResourceManager::listDirectoryFiles(const std::string& directoryPath) {
    std::list<std::string> files;
    auto rc = PHYSFS_enumerateFiles(resolvePath(directoryPath).c_str());

    for(int i = 0; rc[i] != NULL; i++)
        files.push_back(rc[i]);

    PHYSFS_freeList(rc);
    return files;
}

std::string ResourceManager::resolvePath(const std::string& path) {
    std::string fullPath = path;
    stdext::replace_all(fullPath, "//", "/");
    return fullPath;
}

std::string ResourceManager::getRealDir(const std::string& path) {
    std::string dir;
    const char *cdir = PHYSFS_getRealDir(resolvePath(path).c_str());
    if(cdir)
        dir = cdir;
    return dir;
}

std::string ResourceManager::getRealPath(const std::string& path) {
    return getRealDir(path) + "/" + path;
}

std::string ResourceManager::getBaseDir() {
    return PHYSFS_getBaseDir();
}

std::string ResourceManager::getUserDir() {
    return PHYSFS_getUserDir();
}

std::string ResourceManager::guessFilePath(const std::string& filename, const std::string& type) {
    if(isFileType(filename, type))
        return filename;
    return filename + "." + type;
}

bool ResourceManager::isFileType(const std::string& filename, const std::string& type) {
    if(stdext::ends_with(filename, std::string(".") + type))
        return true;
    return false;
}

ticks_t ResourceManager::getFileTime(const std::string& filename) {
	std::string realPath = getRealPath(filename);
	return PHYSFS_getLastModTime(realPath.c_str());
}
