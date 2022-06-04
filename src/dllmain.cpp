#include "includes.h"
#include <libbase64.h>

#ifdef _DEBUG
#define MEASURE_TIME
#define TEST_MATCH_VANILLA
#include <chrono>
#include <iostream>
#include <fstream>
#endif

size_t (__cdecl* base64Decode)(char*, size_t, char**, bool);
size_t __cdecl base64Decode_H(char* in, size_t inLength, char** out, bool url) {
#ifdef MEASURE_TIME

    auto start = std::chrono::system_clock::now();

    char* vanillaOut;
    auto vanillaResult = base64Decode(in, inLength, &vanillaOut, url);

    auto end = std::chrono::system_clock::now();
    std::cout << "vanilla " << inLength << " decode took " << (float)(end - start).count() / 10000.f << "ms" << std::endl;
    auto vanillaTime = (end - start).count();

    start = std::chrono::system_clock::now();

#else

    // use vanilla algorithm on short strings
    if(inLength < 1000)
        return base64Decode(in, inLength, out, url);

#endif

    *out = (char*)malloc(inLength * 3 / 4 + 1);
    size_t result;
    if(url)
        base64_decode_url(in, inLength, *out, &result, 0);
    else
        base64_decode(in, inLength, *out, &result, 0);
    (*out)[result] = 0;

#ifdef MEASURE_TIME

    end = std::chrono::system_clock::now();
    std::cout << "new " << inLength << " decode took " << (float)(end - start).count() / 10000.f << "ms" << std::endl;
    auto newTime = (end - start).count();
    if(newTime < vanillaTime)
        std::cout << "                                        less!!" << std::endl;
    std::cout << std::endl;

#ifdef TEST_MATCH_VANILLA

    bool lengthsMatch = vanillaResult == result;
    bool outputMatches = *out != nullptr && vanillaOut != nullptr;
    if(outputMatches) {
        for(size_t i = 0; i < min(vanillaResult, result); ++i) {
            if((*out)[i] == vanillaOut[i])
                continue;
            outputMatches = false;
            break;
        }
    }
    if(!lengthsMatch || !outputMatches) {
        std::cout << "decoding " << in << std::endl;
        if(!lengthsMatch)
            std::cout << "lengths don't match: vanilla: " << vanillaResult << " new: " << result << std::endl;
        if(!outputMatches)
            std::cout << "outputs don't match" << std::endl;
        std::cout << std::endl;
    }

#endif

#endif

    return result;
}

size_t (__cdecl* base64Encode)(char*, size_t, char**, bool);
size_t __cdecl base64Encode_H(char* in, size_t inLength, char** out, bool url) {
#ifdef MEASURE_TIME

    auto start = std::chrono::system_clock::now();

    char* vanillaOut;
    auto vanillaResult = base64Encode(in, inLength, &vanillaOut, url);

    auto end = std::chrono::system_clock::now();
    std::cout << "vanilla " << inLength << " encode took " << (float)(end - start).count() / 10000.f << "ms" << std::endl;
    auto vanillaTime = (end - start).count();

    start = std::chrono::system_clock::now();

#else

    // use vanilla algorithm on short strings
    if(inLength < 1000)
        return base64Encode(in, inLength, out, url);

#endif

    size_t outLength = inLength * 4 / 3 + (inLength % 3 != 0) * 4;
    *out = (char*)malloc(outLength + 1);
    size_t result;
    if(url)
        base64_encode_url(in, inLength, *out, &result, 0);
    else
        base64_encode(in, inLength, *out, &result, 0);
    (*out)[result] = 0;

#ifdef MEASURE_TIME

    end = std::chrono::system_clock::now();
    std::cout << "new " << inLength << " encode took " << (float)(end - start).count() / 10000.f << "ms" << std::endl;
    auto newTime = (end - start).count();
    if(newTime < vanillaTime)
        std::cout << "                                        less!!" << std::endl;
    std::cout << std::endl;

#ifdef TEST_MATCH_VANILLA

    bool lengthsMatch = vanillaResult == outLength;
    bool outputMatches = *out != nullptr && vanillaOut != nullptr;
    if(outputMatches) {
        for(size_t i = 0; i < min(vanillaResult, result); ++i) {
            if((*out)[i] == vanillaOut[i])
                continue;
            outputMatches = false;
            break;
        }
    }
    if(!lengthsMatch || !outputMatches) {
        std::cout << "encoding" << std::endl;
        if(!lengthsMatch)
            std::cout << "lengths don't match: vanilla: " << vanillaResult << " new: " << outLength << std::endl;
        if(!outputMatches)
            std::cout << "outputs don't match" << std::endl;
        if(vanillaOut) std::cout << "  vanilla: " << vanillaOut << std::endl;
        if(*out) std::cout << "  new: " << *out << std::endl;
        std::cout << std::endl;
    }

#endif

#endif

    return outLength;
}

DWORD WINAPI mainThread(void* hModule) {
#ifdef _DEBUG
    AllocConsole();
    std::ofstream conout("CONOUT$", std::ios::out);
    std::ifstream conin("CONIN$", std::ios::in);
    std::cout.rdbuf(conout.rdbuf());
    std::cin.rdbuf(conin.rdbuf());
#endif

    MH_Initialize();

    auto cocos2dBase = reinterpret_cast<uintptr_t>(GetModuleHandle("libcocos2d.dll"));

    MH_CreateHook(reinterpret_cast<void*>(cocos2dBase + 0xd9cd0), reinterpret_cast<void*>(base64Decode_H),
        reinterpret_cast<void**>(&base64Decode));

    MH_CreateHook(reinterpret_cast<void*>(cocos2dBase + 0xd9d70), reinterpret_cast<void*>(base64Encode_H),
        reinterpret_cast<void**>(&base64Encode));

    MH_EnableHook(MH_ALL_HOOKS);

#ifdef _DEBUG
    std::string input;
    std::getline(std::cin, input);

    MH_Uninitialize();
    conout.close();
    conin.close();
    FreeConsole();
    FreeLibraryAndExitThread((HMODULE)hModule, 0);
#else
    return 0;
#endif
}

BOOL APIENTRY DllMain(HMODULE handle, DWORD reason, LPVOID reserved) {
    if(reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0x100, mainThread, handle, 0, nullptr);
    }
    return TRUE;
}
