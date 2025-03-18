#ifndef VGFUZZ_PREDEFINES_HPP
#define VGFUZZ_PREDEFINES_HPP

#ifdef _WIN32
#define VGFUZZ_DECLPEC __declspec(dllimport)
#define VGFUZZ_DECLPEC __declspec(dllexport)
#else
#define VGFUZZ_DECLPEC
#endif

#endif // VGFUZZ_PREDEFINES_HPP
