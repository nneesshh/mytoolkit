#pragma once

/**
* Represents a random-access file, usually stored on a disk.  Files
* are always binary: that is, they do no end-of-line
* transformations.  File objects are roughly analogous to ANSI C
* FILE* objects.
*
* This interface is not synchronized.
*/
class IRandomAccessFile {
protected:
  ~IRandomAccessFile() { }

public:
/**
 * The different ways you can seek within a file.
 */
  enum SeekMode {
    BEGIN,
    CURRENT,
    END,
  };

/**
 * Read size bytes from the file, storing them in buffer.
 *
 * @param buffer  buffer to read into
 * @param size    number of bytes to read
 *
 * @return  number of bytes successfully read
 */
  MYDLL_METHOD(int) Read(void* buffer, int size) = 0;

  MYDLL_METHOD(int) Write(const void* buffer, int size) = 0;

/**
 * Jump to a new position in the file, using the specified seek
 * mode.  Remember: if mode is END, the position must be negative,
 * to seek backwards from the end of the file into its contents.
 * If the seek fails, the current position is undefined.
 *
 * @param position  position relative to the mode
 * @param mode      where to seek from in the file
 *
 * @return  true on success, false otherwise
 */
  MYDLL_METHOD(bool) Seek(int position, SeekMode mode) = 0;

/**
 * Get current position within the file.
 *
 * @return  current position
 */
  MYDLL_METHOD(int) Tell() = 0;
};

/*EOF*/