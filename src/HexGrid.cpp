/*
    Copyright (C) 2012-2013 by Michael Kristofik <kristo605@gmail.com>
    Part of the libsdl-demos project.
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    or at your option any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY.
 
    See the COPYING.txt file for more details.
*/
#include "HexGrid.h"

#include "algo.h"
#include <cassert>
#include <limits>
#include <random>

HexGrid::HexGrid(int width, int height)
    : width_(width),
    height_(height),
    size_(width_ * height_)
{
    assert(width_ > 0 && height_ > 0);
}

int HexGrid::width() const
{
    return width_;
}

int HexGrid::height() const
{
    return height_;
}

int HexGrid::size() const
{
    return size_;
}

Point HexGrid::hexFromAry(int aIndex) const
{
    if (aIndex < 0 || aIndex >= size_) {
        return hInvalid;
    }

    return {aIndex % width_, aIndex / width_};
}

int HexGrid::aryFromHex(int hx, int hy) const
{
    return aryFromHex({hx, hy});
}

int HexGrid::aryFromHex(const Point &hex) const
{
    if (offGrid(hex)) {
        return -1;
    }

    return hex.y * width_ + hex.x;
}

int HexGrid::aryCorner(Dir d) const
{
    switch (d) {
        case Dir::NW:
            return 0;
        case Dir::NE:
            return width_ - 1;
        case Dir::SE:
            return size_ - 1;
        case Dir::SW:
            return size_ - width_;
        default:
            assert(false);
    }
}

Point HexGrid::hexCorner(Dir d) const
{
    return hexFromAry(aryCorner(d));
}

Point HexGrid::hexRandom() const
{
    static std::uniform_int_distribution<int> dist(0, size_ - 1);
    int aRand = dist(randomGenerator());
    return hexFromAry(aRand);
}

int HexGrid::aryGetNeighbor(int aSrc, Dir d) const
{
    auto neighbor = adjacent(hexFromAry(aSrc), d);
    if (offGrid(neighbor)) {
        return -1;
    }

    return aryFromHex(neighbor);
}

Point HexGrid::hexGetNeighbor(const Point &hSrc, Dir d) const
{
    auto neighbor = adjacent(hSrc, d);
    if (offGrid(neighbor)) {
        return hInvalid;
    }

    return neighbor;
}

std::vector<int> HexGrid::aryNeighbors(int aIndex) const
{
    std::vector<int> av;

    for (auto d : Dir()) {
        auto aNeighbor = aryGetNeighbor(aIndex, d);
        if (aNeighbor != -1) {
            av.push_back(aNeighbor);
        }
    }

    return av;
}

std::vector<Point> HexGrid::hexNeighbors(const Point &hex) const
{
    std::vector<Point> hv;

    for (auto aNeighbor : aryNeighbors(aryFromHex(hex))) {
        hv.push_back(hexFromAry(aNeighbor));
    }

    return hv;
}

bool HexGrid::offGrid(const Point &hex) const
{
    return hex.x < 0 ||
           hex.y < 0 ||
           hex.x >= width_ ||
           hex.y >= height_;
}
