﻿/*
 * Tencent is pleased to support the open source community by making Puerts available.
 * Copyright (C) 2020 Tencent.  All rights reserved.
 * Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may
 * be subject to their corresponding license terms. This file is subject to the terms and conditions defined in file 'LICENSE',
 * which is part of this source code package.
 */

#pragma once

#if USING_IN_UNREAL_ENGINE
#include "CoreMinimal.h"
#include "UObject/Package.h"
#include "UObject/Class.h"
#else
#include "JSClassRegister.h"
#endif

#include "NamespaceDef.h"

PRAGMA_DISABLE_UNDEFINED_IDENTIFIER_WARNINGS
#pragma warning(push, 0)
#include "v8.h"
#pragma warning(pop)
PRAGMA_ENABLE_UNDEFINED_IDENTIFIER_WARNINGS

#if !defined(MAPPER_ISOLATE_DATA_POS)
#define MAPPER_ISOLATE_DATA_POS 2
#endif
#if !defined(BACKENDENV_DATA_POS)
#define BACKENDENV_DATA_POS 1
#endif
#if !defined(PESAPI_PRIVATE_DATA_POS_IN_ISOLATE)
#define PESAPI_PRIVATE_DATA_POS_IN_ISOLATE 3
#endif

#define RELEASED_UOBJECT ((UObject*) 12)
#define RELEASED_UOBJECT_MEMBER ((void*) 12)

#include <sstream>

namespace PUERTS_NAMESPACE
{
template <typename T, typename FT, typename = void>
struct TOuterLinker
{
    V8_INLINE static void Link(v8::Local<v8::Context> Context, v8::Local<v8::Value> Outer, v8::Local<v8::Value> Inner)
    {
    }
};

V8_INLINE void LinkOuterImpl(v8::Local<v8::Context> Context, v8::Local<v8::Value> Outer, v8::Local<v8::Value> Inner)
{
#ifdef WITH_OUTER_LINK
    Inner.As<v8::Object>()->Set(Context, 0, Outer);
#endif
}

#if USING_IN_UNREAL_ENGINE
FORCEINLINE UScriptStruct* GetScriptStructInCoreUObject(const TCHAR* Name)
{
    static UPackage* CoreUObjectPkg = FindObjectChecked<UPackage>(nullptr, TEXT("/Script/CoreUObject"));
    return FindObjectChecked<UScriptStruct>(CoreUObjectPkg, Name);
}

template <class T, typename Enable = void>
struct TScriptStructTraits
{
};

template <>
struct TScriptStructTraits<FVector>
{
    static UScriptStruct* Get()
    {
        return TBaseStructure<FVector>::Get();
    }
};

template <>
struct TScriptStructTraits<FVector2D>
{
    static UScriptStruct* Get()
    {
        return TBaseStructure<FVector2D>::Get();
    }
};

template <>
struct TScriptStructTraits<FVector4>
{
    static UScriptStruct* Get()
    {
        return TBaseStructure<FVector4>::Get();
    }
};

template <>
struct TScriptStructTraits<FRotator>
{
    static UScriptStruct* Get()
    {
        return TBaseStructure<FRotator>::Get();
    }
};

template <>
struct TScriptStructTraits<FQuat>
{
    static UScriptStruct* Get()
    {
        return TBaseStructure<FQuat>::Get();
    }
};

template <>
struct TScriptStructTraits<FTransform>
{
    static UScriptStruct* Get()
    {
        return TBaseStructure<FTransform>::Get();
    }
};

template <>
struct TScriptStructTraits<FLinearColor>
{
    static UScriptStruct* Get()
    {
        return TBaseStructure<FLinearColor>::Get();
    }
};

template <>
struct TScriptStructTraits<FColor>
{
    static UScriptStruct* Get()
    {
        return TBaseStructure<FColor>::Get();
    }
};

template <>
struct TScriptStructTraits<FGuid>
{
    static UScriptStruct* Get()
    {
        return TBaseStructure<FGuid>::Get();
    }
};

template <>
struct TScriptStructTraits<FBox2D>
{
    static UScriptStruct* Get()
    {
        return TBaseStructure<FBox2D>::Get();
    }
};

template <>
struct TScriptStructTraits<FIntPoint>
{
    static UScriptStruct* Get()
    {
        return GetScriptStructInCoreUObject(TEXT("IntPoint"));
    }
};

template <>
struct TScriptStructTraits<FIntVector>
{
    static UScriptStruct* Get()
    {
        return GetScriptStructInCoreUObject(TEXT("IntVector"));
    }
};

template <>
struct TScriptStructTraits<FPlane>
{
    static UScriptStruct* Get()
    {
        return GetScriptStructInCoreUObject(TEXT("Plane"));
    }
};

template <>
struct TScriptStructTraits<FMatrix>
{
    static UScriptStruct* Get()
    {
        return GetScriptStructInCoreUObject(TEXT("Matrix"));
    }
};

template <>
struct TScriptStructTraits<FIntVector4>
{
    static UScriptStruct* Get()
    {
        return GetScriptStructInCoreUObject(TEXT("IntVector4"));
    }
};

template <class...>
using ToVoid = void;

template <typename T, typename = void>
struct HasStaticStructHelper : std::false_type
{
};

template <typename T>
struct HasStaticStructHelper<T, ToVoid<decltype(&T::StaticStruct)>> : std::true_type
{
};

template <typename T>
struct TScriptStructTraits<T, typename std::enable_if<HasStaticStructHelper<T>::value>::type>
{
    static UScriptStruct* Get()
    {
        return T::StaticStruct();
    }
};

template <typename T, typename FT>
struct TOuterLinker<T, FT, ToVoid<decltype(&TScriptStructTraits<FT>::Get)>>
{
    V8_INLINE static void Link(v8::Local<v8::Context> Context, v8::Local<v8::Value> Outer, v8::Local<v8::Value> Inner)
    {
        LinkOuterImpl(Context, Outer, Inner);
    }
};
#endif

class REGISTER_API DataTransfer
{
public:
    FORCEINLINE static void* MakeAddressWithHighPartOfTwo(void* Address1, void* Address2)
    {
        UPTRINT High = reinterpret_cast<UPTRINT>(Address1) & (((UPTRINT) -1) << (sizeof(UPTRINT) / 2));    //清除低位
        UPTRINT Low = (reinterpret_cast<UPTRINT>(Address2) >> (sizeof(UPTRINT) / 2)) &
                      ~(((UPTRINT) -1) << (sizeof(UPTRINT) / 2));    //右移，然后清除高位
        return reinterpret_cast<void*>(High | Low);
    }

    FORCEINLINE static void SplitAddressToHighPartOfTwo(const void* Address, UPTRINT& High, UPTRINT& Low)
    {
        High = reinterpret_cast<UPTRINT>(Address) & (((UPTRINT) -1) << (sizeof(UPTRINT) / 2));    //清除低位
        Low = reinterpret_cast<UPTRINT>(Address) << (sizeof(UPTRINT) / 2);
    }

    template <typename T>
    FORCEINLINE static T* GetPointerFast(v8::Local<v8::Object> Object, int Index)
    {
        int P1 = Index << 1;
        int P2 = P1 + 1;
        if (V8_LIKELY(Object->InternalFieldCount() > P2))
        {
            return static_cast<T*>(MakeAddressWithHighPartOfTwo(
                Object->GetAlignedPointerFromInternalField(P1), Object->GetAlignedPointerFromInternalField(P2)));
        }
        return nullptr;
    }

    template <typename T>
    FORCEINLINE static T* GetPointerFast(v8::Local<v8::Object> Object)
    {
        if (V8_LIKELY(Object->InternalFieldCount() > 1))
        {
            return static_cast<T*>(MakeAddressWithHighPartOfTwo(
                Object->GetAlignedPointerFromInternalField(0), Object->GetAlignedPointerFromInternalField(1)));
        }
        return nullptr;
    }
    
    template <typename T>
    FORCEINLINE static T* GetPointer(v8::Local<v8::Object> Object, int Index = 0)
    {
        if (Object.IsEmpty() || Object->IsUndefined() || Object->IsNull())
        {
            return nullptr;
        }
        return GetPointerFast<T>(Object, Index);
    }
    
    template <typename T>
    FORCEINLINE static T* GetPointer(v8::Local<v8::Context>& Context, v8::Local<v8::Value> Value, int Index = 0)
    {
        if (Value.IsEmpty() || !Value->IsObject() || Value->IsUndefined() || Value->IsNull())
        {
            return nullptr;
        }
        auto Object = Value->ToObject(Context).ToLocalChecked();
        return GetPointerFast<T>(Object, Index);
    }

    //替代 Object->SetAlignedPointerInInternalField(Index, Ptr);
    FORCEINLINE static void SetPointer(v8::Isolate* Isolate, v8::Local<v8::Object> Object, const void* Ptr, int Index)
    {
        // Object->SetInternalField(Index, v8::External::New(Isolate, Ptr));
        // Object->SetAlignedPointerInInternalField(Index, Ptr);
        UPTRINT High;
        UPTRINT Low;
        SplitAddressToHighPartOfTwo(Ptr, High, Low);
        Object->SetAlignedPointerInInternalField(Index * 2, reinterpret_cast<void*>(High));
        Object->SetAlignedPointerInInternalField(Index * 2 + 1, reinterpret_cast<void*>(Low));
    }
    
    FORCEINLINE static void SetPointer(v8::Object* Object, const void* Ptr, int Index)
    {
        UPTRINT High;
        UPTRINT Low;
        SplitAddressToHighPartOfTwo(Ptr, High, Low);
        Object->SetAlignedPointerInInternalField(Index * 2, reinterpret_cast<void*>(High));
        Object->SetAlignedPointerInInternalField(Index * 2 + 1, reinterpret_cast<void*>(Low));
    }

    template <typename T>
    FORCEINLINE static T* IsolateData(v8::Isolate* Isolate)
    {
        return static_cast<T*>(Isolate->GetData(MAPPER_ISOLATE_DATA_POS));
    }

    FORCEINLINE static void* GetIsolatePrivateData(v8::Isolate* Isolate)
    {
        return Isolate->GetData(PESAPI_PRIVATE_DATA_POS_IN_ISOLATE);
    }

    FORCEINLINE static void SetIsolatePrivateData(v8::Isolate* Isolate, void* PrivateData)
    {
        Isolate->SetData(PESAPI_PRIVATE_DATA_POS_IN_ISOLATE, PrivateData);
    }

    static v8::Local<v8::Value> FindOrAddCData(
        v8::Isolate* Isolate, v8::Local<v8::Context> Context, const void* TypeId, const void* Ptr, bool PassByPointer);

    static bool IsInstanceOf(v8::Isolate* Isolate, const void* TypeId, v8::Local<v8::Value> JsObject);

    static v8::Local<v8::Value> UnRef(v8::Isolate* Isolate, const v8::Local<v8::Value>& Value);

    static void UpdateRef(v8::Isolate* Isolate, v8::Local<v8::Value> Outer, const v8::Local<v8::Value>& Value);

    static std::weak_ptr<int> GetJsEnvLifeCycleTracker(v8::Isolate* Isolate);

#if USING_IN_UNREAL_ENGINE
    template <typename T>
    static v8::Local<v8::Value> FindOrAddObject(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, T* UEObject)
    {
        return FindOrAddObject(Isolate, Context, UEObject == nullptr ? T::StaticClass() : UEObject->GetClass(), UEObject);
    }

    static v8::Local<v8::Value> FindOrAddObject(
        v8::Isolate* Isolate, v8::Local<v8::Context>& Context, UClass* Class, UObject* UEObject);

    template <typename T>
    static v8::Local<v8::Value> FindOrAddStruct(v8::Isolate* Isolate, v8::Local<v8::Context> Context, void* Ptr, bool PassByPointer)
    {
        return FindOrAddStruct(Isolate, Context, TScriptStructTraits<T>::Get(), Ptr, PassByPointer);
    }

    static v8::Local<v8::Value> FindOrAddStruct(
        v8::Isolate* Isolate, v8::Local<v8::Context> Context, UScriptStruct* ScriptStruct, void* Ptr, bool PassByPointer);

    template <typename T>
    static bool IsInstanceOf(v8::Isolate* Isolate, v8::Local<v8::Value> JsObject)
    {
        return IsInstanceOf(Isolate, TScriptStructTraits<T>::Get(), JsObject);
    }

    static bool IsInstanceOf(v8::Isolate* Isolate, UStruct* Struct, v8::Local<v8::Value> JsObject);

    static FString ToFString(v8::Isolate* Isolate, v8::Local<v8::Value> Value);

    static void ThrowException(v8::Isolate* Isolate, const char* Message);
#endif

    template <typename T1, typename T2>
    FORCEINLINE static void LinkOuter(v8::Local<v8::Context> Context, v8::Local<v8::Value> Outer, v8::Local<v8::Value> Inner)
    {
        TOuterLinker<T1, T2>::Link(Context, Outer, Inner);
    }

    FORCEINLINE static v8::Local<v8::ArrayBuffer> NewArrayBuffer(v8::Local<v8::Context> Context, void* Data, size_t DataLength)
    {
#if USING_IN_UNREAL_ENGINE
        return v8::ArrayBuffer::New(Context->GetIsolate(), Data, DataLength);
#else
        v8::Local<v8::ArrayBuffer> Ab = v8::ArrayBuffer::New(Context->GetIsolate(), DataLength);
        void* Buff = Ab->GetBackingStore()->Data();
        ::memcpy(Buff, Data, DataLength);
        return Ab;
#endif
    }

    FORCEINLINE static void* GetArrayBufferData(v8::Local<v8::ArrayBuffer> InArrayBuffer)
    {
        size_t DataLength;
        return GetArrayBufferData(InArrayBuffer, DataLength);
    }

    FORCEINLINE static void* GetArrayBufferData(v8::Local<v8::ArrayBuffer> InArrayBuffer, size_t& DataLength)
    {
#if defined(HAS_ARRAYBUFFER_NEW_WITHOUT_STL)
        return v8::ArrayBuffer_Get_Data(InArrayBuffer, DataLength);
#else
#if USING_IN_UNREAL_ENGINE
        DataLength = InArrayBuffer->GetContents().ByteLength();
        return InArrayBuffer->GetContents().Data();
#else
        auto BS = InArrayBuffer->GetBackingStore();
        DataLength = BS->ByteLength();
        return BS->Data();
#endif
#endif
    }
    
    FORCEINLINE static void ThrowException(v8::Isolate* Isolate, const char* Message)
    {
        auto ExceptionStr = v8::String::NewFromUtf8(Isolate, Message, v8::NewStringType::kNormal).ToLocalChecked();
        Isolate->ThrowException(v8::Exception::Error(ExceptionStr));
    }
    
    FORCEINLINE static std::string ExceptionToString(v8::Isolate* Isolate, v8::Local<v8::Value> ExceptionValue)
    {
        v8::String::Utf8Value Exception(Isolate, ExceptionValue);
        const char * StrException = *Exception;
        std::string ExceptionStr(StrException == nullptr ? "" : StrException);
        v8::Local<v8::Message> Message = v8::Exception::CreateMessage(Isolate, ExceptionValue);
        if (Message.IsEmpty())
        {
            // 如果没有提供更详细的信息，直接输出Exception
            return ExceptionStr;
        }
        else
        {
            v8::Local<v8::Context> Context(Isolate->GetCurrentContext());

            // 输出 (filename):(line number): (message).
            std::ostringstream stm;
            v8::String::Utf8Value FileName(Isolate, Message->GetScriptResourceName());
            int LineNum = Message->GetLineNumber(Context).FromJust();
            const char * StrFileName = *FileName;
            stm << (StrFileName == nullptr ? "unknow file" : StrFileName) << ":" << LineNum << ": " << ExceptionStr;

            stm << std::endl;

            // 输出调用栈信息
            v8::MaybeLocal<v8::Value> MaybeStackTrace = v8::TryCatch::StackTrace(Context, ExceptionValue);
            if (!MaybeStackTrace.IsEmpty())
            {
                v8::String::Utf8Value StackTraceVal(Isolate, MaybeStackTrace.ToLocalChecked());
                stm << std::endl << *StackTraceVal;
            }
            return stm.str();
        }
    }
};
}    // namespace PUERTS_NAMESPACE
