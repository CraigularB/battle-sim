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
#ifndef HEX_GRID_H
#define HEX_GRID_H

#include "hex_utils.h"
#include <vector>

// Logical view of a hex grid.  Designed to be cheap to create, copy, etc.
class HexGrid
{
public:
    HexGrid(int width, int height);

    int width() const;
    int height() const;
    int size() const;

    // Two ways to view a hex map: a 2D map of (x,y) coordinates, and a
    // contiguous array.  These functions convert between the two
    // representations.
    Point hexFromAry(int aIndex) const;
    int aryFromHex(int hx, int hy) const;
    int aryFromHex(const Point &hex) const;

    // Accessors for the four corners of the grid.
    int aryCorner(Dir d) const;
    Point hexCorner(Dir d) const;

    // Generate a random hex in the range [(0,0), (width-1,height-1)].
    Point hexRandom() const;

    // Return the neighbor hex in a given direction from the source hex.
    // Return -1/invalid if the neighbor hex would be off the map.
    int aryGetNeighbor(int aSrc, Dir d) const;
    Point hexGetNeighbor(const Point &hSrc, Dir d) const;

    // Compute all neighbors of a given hex.  Might have fewer than 6.
    std::vector<int> aryNeighbors(int aIndex) const;
    std::vector<Point> hexNeighbors(const Point &hex) const;

    // Return true if hex is outside the grid boundary.
    bool offGrid(const Point &hex) const;

private:
    int width_;
    int height_;
    int size_;
};

#endif
