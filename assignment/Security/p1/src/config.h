#ifndef CONFIG_H
#define CONFIG_H

#ifdef _MSC_VER
#ifdef NETWORK_LIBRARY
#define DLL_SYMBOL __declspec(dllexport)
#else
#define DLL_SYMBOL __declspec(dllimport)
#endif
#else
#define DLL_SYMBOL
#endif

#endif
