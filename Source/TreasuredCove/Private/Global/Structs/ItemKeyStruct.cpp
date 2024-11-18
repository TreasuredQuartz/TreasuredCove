// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemKeyStruct.h"
#include "Misc/Crc.h"

#if UE_BUILD_DEBUG
uint32 GetTypeHash(const FItemKey& Key)
{
    uint32 Hash = FCrc::MemCrc32(&Key, sizeof(FItemKey));
    return Hash;
}
#endif