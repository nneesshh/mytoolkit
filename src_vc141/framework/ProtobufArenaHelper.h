#pragma once
//------------------------------------------------------------------------------
/**
@class CProtobufArenaHelper

(C) 2016 n.lee
*/
#include "../UsingProtobuf.h"

//------------------------------------------------------------------------------
/**
@brief CProtobufArenaHelper
*/
class CProtobufArenaHelper {
public:
	CProtobufArenaHelper();
	virtual ~CProtobufArenaHelper();

	template<typename T>
	T *	GetProto(const std::string& sProtoName) {
		T *pProto = nullptr;
		auto& iter = _mapProto.find(sProtoName);
		if (iter != _mapProto.end()) {
			pProto = static_cast<T *>(iter->second);
			pProto->Clear();
		}
		else {
			// Allocate a protobuf message in the arena.
			pProto = google::protobuf::Arena::CreateMessage<T>(&_arena);

			// cache
			_mapProto[sProtoName] = pProto;
		}
		return pProto;
	}

	google::protobuf::Arena _arena;
	std::unordered_map<std::string, google::protobuf::Message *> _mapProto;
};

/*EOF*/