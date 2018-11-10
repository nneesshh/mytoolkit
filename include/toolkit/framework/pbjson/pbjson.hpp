#ifndef PBJSON_HPP_
#define PBJSON_HPP_

#include <string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "../xml2json/include/rapidjson/document.h"

#define ERR_INVALID_ARG -1
#define ERR_INVALID_PB -2
#define ERR_UNKNOWN_FIELD -3
#define ERR_INVALID_JSON -4

namespace pbjson {
    void pb2json(const google::protobuf::Message* msg, std::string& str);
    rapidjson::Value* pb2jsonobject(const google::protobuf::Message* msg);
    rapidjson::Value* pb2jsonobject(const google::protobuf::Message* msg, rapidjson::Value::AllocatorType& allocator);
    void json2string(const rapidjson::Value* json, std::string& str);

    int json2pb(const std::string& json, google::protobuf::Message* msg, std::string& err);
    int jsonobject2pb(const rapidjson::Value* json, google::protobuf::Message* msg, std::string& err);
}


#endif /* PBJSON_HPP_ */
