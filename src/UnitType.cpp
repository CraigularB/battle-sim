/*
    Copyright (C) 2013 by Michael Kristofik <kristo605@gmail.com>
    Part of the battle-sim project.
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    or at your option any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY.
 
    See the COPYING.txt file for more details.
*/
#include "UnitType.h"
#include <algorithm>
#include <iterator>

UnitType::UnitType(const rapidjson::Value &json)
    : name{},
    plural{},
    moves{1},
    hp{1},
    hasRangedAttack{false},
    minDmg{1},
    maxDmg{1},
    minDmgRanged{0},
    maxDmgRanged{0},
    baseImg{},
    reverseImg{},
    animAttack{},
    reverseAnimAttack{},
    attackFrames{},
    animRanged{},
    reverseAnimRanged{},
    rangedFrames{},
    projectile{},
    imgDefend{},
    reverseImgDefend{},
    animDie{},
    reverseAnimDie{},
    dieFrames{}
{
    if (json.HasMember("name")) {
        name = json["name"].GetString();
    }
    if (json.HasMember("plural")) {
        plural = json["plural"].GetString();
    }
    if (json.HasMember("moves")) {
        moves = json["moves"].GetInt();
    }
    if (json.HasMember("hp")) {
        hp = json["hp"].GetInt();
    }
    if (json.HasMember("img")) {
        auto img = sdlLoadImage(json["img"].GetString());
        if (img) {
            baseImg = applyTeamColors(img);
            reverseImg = applyTeamColors(sdlFlipH(img));
        }
    }
    if (json.HasMember("img-move")) {
        auto img = sdlLoadImage(json["img-move"].GetString());
        if (img) {
            imgMove = applyTeamColors(img);
            reverseImgMove = applyTeamColors(sdlFlipH(img));
        }
    }
    if (json.HasMember("img-defend")) {
        auto img = sdlLoadImage(json["img-defend"].GetString());
        if (img) {
            imgDefend = applyTeamColors(img);
            reverseImgDefend = applyTeamColors(sdlFlipH(img));
        }
    }
    if (json.HasMember("anim-attack") && json.HasMember("attack-frames")) {
        const auto &frameList = json["attack-frames"];
        transform(frameList.Begin(),
                  frameList.End(),
                  std::back_inserter(attackFrames),
                  [&] (const rapidjson::Value &elem) { return elem.GetInt(); });

        auto baseAnim = sdlLoadImage(json["anim-attack"].GetString());
        if (baseAnim) {
            animAttack = applyTeamColors(baseAnim);
            reverseAnimAttack = applyTeamColors(sdlFlipSheetH(baseAnim,
                attackFrames.size()));
        }
    }
    if (json.HasMember("anim-ranged") && json.HasMember("ranged-frames")) {
        const auto &frameList = json["ranged-frames"];
        transform(frameList.Begin(),
                  frameList.End(),
                  std::back_inserter(rangedFrames),
                  [&] (const rapidjson::Value &elem) { return elem.GetInt(); });

        auto baseAnim = sdlLoadImage(json["anim-ranged"].GetString());
        if (baseAnim) {
            animRanged = applyTeamColors(baseAnim);
            reverseAnimRanged = applyTeamColors(sdlFlipSheetH(baseAnim,
                rangedFrames.size()));
        }
    }
    if (json.HasMember("damage")) {
        const auto &damageList = json["damage"];
        minDmg = damageList[0u].GetInt();
        maxDmg = damageList[1u].GetInt();
    }
    if (json.HasMember("damage-ranged")) {
        hasRangedAttack = true;

        const auto &damageList = json["damage-ranged"];
        minDmgRanged = damageList[0u].GetInt();
        maxDmgRanged = damageList[1u].GetInt();
    }
    if (json.HasMember("projectile")) {
        projectile = sdlLoadImage(json["projectile"].GetString());
    }
    if (json.HasMember("anim-die") && json.HasMember("die-frames")) {
        // TODO: refactor this
        const auto &frameList = json["die-frames"];
        transform(frameList.Begin(),
                  frameList.End(),
                  std::back_inserter(dieFrames),
                  [&] (const rapidjson::Value &elem) { return elem.GetInt(); });

        auto baseAnim = sdlLoadImage(json["anim-die"].GetString());
        if (baseAnim) {
            animDie = applyTeamColors(baseAnim);
            reverseAnimDie = applyTeamColors(sdlFlipSheetH(baseAnim,
                dieFrames.size()));
        }
    }
}