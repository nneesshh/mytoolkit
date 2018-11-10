#include "binarytree.h"
#include "bytestream.h"

BinaryTreeNodePtr BinaryTreeNode::getFirstChild() {
	if (_curChild)
		stdext::throw_exception("current child is not null");

	if (_tree->_lastWasStart) {
		_curChild = new BinaryTreeNode(_tree, this);
		_tree->loadData(*_curChild);
		return _curChild;
	}
	else {
		return nullptr;
	}
}

BinaryTreeNodePtr BinaryTreeNode::advance() {
	if (!_curChild)
		_curChild = getFirstChild().get();

	while (_curChild) {
		_curChild->advance();
	}

	if (_tree->_lastWasStart) {
		return nullptr;
	}
	else {
		uint8 op = _tree->_streamIn->getU8();
		if (op == BINARYTREE_NODE_START) {
			// Another node follows this.
			// Load this node as the next one
			BinaryTreeNodePtr sibling = this;
			_tree->loadData(*sibling);
			return sibling;
		}
		else if (op == BINARYTREE_NODE_END) {
			// End of this child-tree
			_parent->_curChild = nullptr;
			_tree->_lastWasStart = false;
			return nullptr;
		}
		else {
			return nullptr;
		}
	}
}

BinaryTree::BinaryTree(const ByteStreamPtr& streamIn) :
    _streamIn(streamIn),
	_startPos(streamIn->tell()),
	_lastWasStart(true) {

}

BinaryTree::~BinaryTree() {
}

BinaryTreeNodePtr BinaryTree::getRootNode() {
	auto op = _streamIn->getU8();
	if (op != BINARYTREE_NODE_START) {
		stdext::throw_exception("getBinaryTree: failed to read node start flag!");
	}

	BinaryTreeNodePtr rootNode = new BinaryTreeNode(this, nullptr);
	loadData(*rootNode);
	return rootNode;
}

void BinaryTree::loadData(BinaryTreeNode& node) {
	Uint8ArrayPtr arr = new Uint8Array();
	uint8 op = 0;

	while (true) {
		op = _streamIn->getU8();
		switch (op) {
		case BINARYTREE_NODE_START: {
			node._buffer = new ByteStream(arr);
			_lastWasStart = true;
			return;
		}

		case BINARYTREE_NODE_END: {
			node._buffer = new ByteStream(arr);
			_lastWasStart = false;
			return;
		}

		case BINARYTREE_ESCAPE_CHAR: {
			arr->add(_streamIn->getU8());
			break;
		}

		default:
			arr->add(op);
		}
	}
};