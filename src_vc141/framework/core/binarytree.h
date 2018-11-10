#ifndef BINARYTREE_H
#define BINARYTREE_H

#include "declarations.h"
#include "bytestream.h"

enum {
    BINARYTREE_ESCAPE_CHAR = 0xFD,
    BINARYTREE_NODE_START = 0xFE,
    BINARYTREE_NODE_END = 0xFF
};

class BinaryTreeNode : public stdext::shared_object {
public:
	BinaryTreeNode(BinaryTree *tree, BinaryTreeNode *parent) :
		_tree(tree),
		_parent(parent) {

	}
	~BinaryTreeNode() {}

	void seek(uint pos) { return _buffer->seek(pos); }
	void skip(uint len) { seek(tell() + len); }
	uint tell() { return _buffer->tell(); }
	uint size() { return _buffer->size(); }
	bool canRead() { return tell() + 1 < size(); }

	uint8 getU8() { return _buffer->getU8(); }
	uint16 getU16() { return _buffer->getU16(); }
	uint32 getU32() { return _buffer->getU32(); }
	uint64 getU64() { return _buffer->getU64(); }
	std::string getString(uint16 len = 0) { return _buffer->getString(len); }
	uint8 getPointX() { return _buffer->getPointX(); }
	uint8 getPointY() { return _buffer->getPointY(); }

	BinaryTreeNodePtr getFirstChild();
	BinaryTreeNodePtr advance();

	BinaryTreeNodePtr asBinaryTreeNode() { return static_self_cast<BinaryTreeNode>(); }

	BinaryTree *_tree;
	BinaryTreeNode *_parent;

	ByteStreamPtr _buffer = nullptr;
	BinaryTreeNodePtr _curChild = nullptr;
};

class BinaryTree : public stdext::shared_object {
public:
	BinaryTree(const ByteStreamPtr& streamIn);
    ~BinaryTree();

	BinaryTreeNodePtr getRootNode();
	void loadData(BinaryTreeNode& node);

    ByteStreamPtr _streamIn;
	uint _startPos;
	uint _lastWasStart;
};

#endif

