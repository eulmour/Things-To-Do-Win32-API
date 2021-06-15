#pragma once

struct Position
{
    DWORD x;
    DWORD y;
    DWORD width;
    DWORD height;
};

template <typename T, typename ...Ts>
using are_same = std::conjunction<std::is_same<T, Ts>...>;