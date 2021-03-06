/*
    Copyright (C) 2013-2014 by Michael Kristofik <kristo605@gmail.com>
    Part of the battle-sim project.
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    or at your option any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY.
 
    See the COPYING.txt file for more details.
*/
#include "CommanderView.h"

#include "Commander.h"
#include "GameState.h"
#include "boost/lexical_cast.hpp"
#include <string>

CommanderView::CommanderView(SDL_Rect dispArea, int team, const GameState &gs)
    : gs_(gs),
    team_{team},
    font_(sdlGetFont(FontType::MEDIUM)),
    displayArea_(std::move(dispArea)),
    txtAlign_{team == 0 ? Justify::LEFT : Justify::RIGHT}
{
}

void CommanderView::draw() const
{
    sdlClear(displayArea_);

    const auto &cmdr = gs_.getCommander(team_);

    // Draw the portrait.
    auto imgHeight = 200;
    if (cmdr.portrait) {
        auto img = cmdr.portrait;
        imgHeight = img->h;
        if (txtAlign_ == Justify::RIGHT) {
            img = sdlFlipH(img);
        }
        sdlBlit(img, displayArea_.x, displayArea_.y);
    }

    // Draw the name below it.
    auto lineHeight = sdlLineHeight(font_);
    SDL_Rect txtArea;
    txtArea.x = displayArea_.x + 5;  // get away from the screen edges a bit
    txtArea.y = displayArea_.y + imgHeight;
    txtArea.w = displayArea_.w - 10;
    txtArea.h = lineHeight;
    auto title = cmdr.name + " (" + cmdr.alignment + ")";
    sdlDrawText(font_, title, txtArea, WHITE, txtAlign_);

    // Draw the stats below the name.
    txtArea.y += lineHeight;
    std::ostringstream stats;
    stats << "Att: " << cmdr.attack << "  Def: " << cmdr.defense <<
        "  Mana: " << gs_.getManaLeft(team_) << "/" << gs_.getMana(team_);
    sdlDrawText(font_, stats.str(), txtArea, WHITE, txtAlign_);
}
