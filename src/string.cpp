/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "string.h"

std::wstring toWString(const std::string& s)
{
    int length;
    int slength = (int)s.length() + 1;
    length = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, 0, 0);
    std::wstring buf;
    buf.resize(length);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, const_cast<wchar_t*>(buf.c_str()), length);
    return buf;
}