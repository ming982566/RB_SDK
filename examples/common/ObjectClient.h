#pragma once
#include "RaceBearSDK.h"
#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>

// 示例自己的 C++ 包装，不是 DLL 导出；所有临时参数和结果由当前调用独占。
namespace rb_example {
// 标量和本机指针按位复制，不能把 double 转成整数数值或持久化指针。
template<class T> uint64_t Encode(T value) {
    static_assert(std::is_trivially_copyable<T>::value && sizeof(T) <= 8, "Invalid native argument");
    uint64_t bits = 0;
    std::memcpy(&bits, &value, sizeof(T));
    return bits;
}

// 传输失败不能伪装成合法的 0、false 或空指针，由宿主最外层报告错误。
inline void CheckTransport(int rc, const char* operation) {
    if (rc != RB_OK)
        throw std::runtime_error(std::string(operation) + ": transport code=" + std::to_string(rc));
}

// 同步调用只执行一次；返回业务原值，业务返回码仍由调用方检查。
template<class Result, class... Args>
Result Native(const char* domain, const char* operation, uint64_t signature, Args... args) {
    static_assert(sizeof...(Args) <= 16, "Too many native arguments");
    int id = 0;
    CheckTransport(RB_Object_Resolve(domain, &id), operation);
    uint64_t bits = 0, revision = 0;
    RB_ObjectNativeCall call{};
    call.Size = sizeof(call);
    call.Version = 1;
    call.PointerBytes = sizeof(void*);
    call.Operation = operation;
    call.Signature = signature;
    call.ArgumentCount = sizeof...(Args);
    call.Result = &bits;
    const uint64_t encoded[] = {0, Encode(args)...};
    for (size_t i = 0; i < sizeof...(Args); ++i) call.Arguments[i] = encoded[i + 1];
    CheckTransport(RB_Object_Write(id, RB_OBJECT_NATIVE_CALL, &call, sizeof(call), 0, &revision), operation);
    if constexpr (!std::is_void<Result>::value) {
        Result value{};
        static_assert(sizeof(value) <= sizeof(bits), "Invalid native result");
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }
}

// 二进制状态直接读对象；失败时清空调用方帧，避免把历史值显示为当前值。
template<class T> int ReadBinary(const char* key, T& value, RB_ObjectReadInfo* details = nullptr) {
    value = {};
    int id = 0;
    int rc = RB_Object_Resolve(key, &id);
    if (rc != RB_OK) return rc;
    RB_ObjectReadInfo info{};
    info.Size = sizeof(info); info.Version = 1;
    rc = RB_Object_Read(id, RB_OBJECT_BINARY, &value, sizeof(value), &info);
    if (rc != RB_OK) value = {};
    if (details) *details = info;
    return rc;
}

}
