#ifndef FRAMEWORK_CORE_DECLARATIONS_H
#define FRAMEWORK_CORE_DECLARATIONS_H

#include <vector>
#include <string>
#include "framework/stdext/stdext.h"

class ConfigManager;
class ModuleManager;
class ResourceManager;

class Module;
class FileStream;
class ByteStream;
class BinaryTreeNode;
class BinaryTree;

typedef stdext::shared_object_ptr<Module> ModulePtr;
typedef stdext::shared_object_ptr<FileStream> FileStreamPtr;
typedef stdext::shared_object_ptr<ByteStream> ByteStreamPtr;
typedef stdext::shared_object_ptr<BinaryTreeNode> BinaryTreeNodePtr;
typedef stdext::shared_object_ptr<BinaryTree> BinaryTreePtr;

typedef DataBuffer<uint8> Uint8Array;
typedef stdext::shared_object_ptr<Uint8Array> Uint8ArrayPtr;

#endif
