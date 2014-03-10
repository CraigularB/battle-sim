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
#include "Effects.h"

#include "Action.h"
#include "GameState.h"
#include "Unit.h"
#include "UnitType.h"
#include "algo.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <unordered_map>

namespace
{
    std::unordered_map<int, std::unique_ptr<EffectData>> cache;

    const EffectData * getData(EffectType type)
    {
        auto iter = cache.find(static_cast<int>(type));
        assert(iter != cache.end());
        return iter->second.get();
    }
}


EffectBound::EffectBound()
    : EffectData{}
{
    type = EffectType::BOUND;
    dur = Duration::UNTIL_ATT_MOVES;
}

void EffectBound::apply(GameState &gs, Effect &effect, Unit &unit) const
{
    const int &attId = effect.data1;
    const int &attHex = effect.data2;

    const auto &attacker = gs.getUnit(attId);
    if (!attacker.isAlive() || attacker.aHex != attHex) {
        effect.dispose();
        return;
    }
}

Effect EffectBound::create(const GameState &gs, const Action &action) const
{
    const auto &att = gs.getUnit(action.attacker);

    Effect e;
    e.type = EffectType::BOUND;
    e.roundsLeft = 1;
    e.data1 = att.entityId;
    e.data2 = att.aHex;
    return e;
}


EffectHeal::EffectHeal()
    : EffectData{}
{
    type = EffectType::HEAL;
    // TODO: make this configurable
    anim = sdlLoadImage("regeneration.png");
    animFrames = {75, 150, 225, 300, 375, 450, 525, 600};
    dur = Duration::INSTANT;
    text = "healed";
}

void EffectHeal::apply(GameState &gs, Effect &effect, Unit &unit) const
{
    const int hpGained = effect.data1;
    unit.hpLeft += hpGained;
    unit.hpLeft = std::min(unit.hpLeft, unit.type->hp);
    effect.dispose();
}

Effect EffectHeal::create(const GameState &gs, const Action &action) const
{
    Effect e;
    e.type = EffectType::HEAL;
    e.roundsLeft = 1;
    e.data1 = action.damage;
    return e;
}


Effect::Effect()
    : type{EffectType::NONE},
    roundsLeft{0},
    data1{0},
    data2{0}
{
}

Effect::Effect(const GameState &gs, const Action &action, EffectType type)
    : Effect{}
{
    *this = getData(type)->create(gs, action);
}

const SdlSurface & Effect::getAnim() const
{
    return getData(type)->anim;
}

const FrameList & Effect::getFrames() const
{
    return getData(type)->animFrames;
}

const std::string & Effect::getText() const
{
    return getData(type)->text;
}

bool Effect::isDone() const
{
    return roundsLeft <= 0;
}

void Effect::apply(GameState &gs, Unit &unit)
{
    getData(type)->apply(gs, *this, unit);
}

void Effect::dispose()
{
    roundsLeft = 0;
}


void initEffectCache()
{
    cache.emplace(static_cast<int>(EffectType::BOUND),
                  make_unique<EffectBound>());
    cache.emplace(static_cast<int>(EffectType::HEAL),
                  make_unique<EffectHeal>());
}