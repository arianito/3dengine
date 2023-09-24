#pragma once

#include <type_traits>

#include "mem/alloc.h"
#include "engine/Memory.hpp"
#include "data/TString.hpp"
#include "data/TFlatMap.hpp"
#include "engine/Level.hpp"

template<class TAlloc = FreeListMemory>
class LevelManager {
private:
    TFlatMap<LevelId, Level *, TAlloc> mLevels;
    Level *mPreviousLevel = nullptr;
    Level *mCurrentLevel = nullptr;
public:
    inline ~LevelManager() {
        if (mCurrentLevel)
            mCurrentLevel->Destroy();
        for (const auto &level: mLevels) {
            Free<FreeListMemory>(&level.second);
        }
    }

    template<typename T>
    inline void Add() {
        static_assert(std::is_base_of_v<Level, T>, "Level Manager: class must be type of Level");
        auto id = GetLevelTypeId<T>();
        if (mLevels.Contains(id)) return;
        Level *level = AllocNew<TAlloc, T>();
        mLevels.Set(id, level);
    }

    template<typename T>
    inline void Load() {
        auto id = GetLevelTypeId<T>();
        if (!mLevels.Contains(id)) return;
        mCurrentLevel = mLevels[id];
    }

    inline void Update() {
        if (!mCurrentLevel) return;
        if (mCurrentLevel != mPreviousLevel) {
            if (mPreviousLevel) {
                mPreviousLevel->Destroy();
                mPreviousLevel = nullptr;
            }

            if (mCurrentLevel) {
                mCurrentLevel->Create();
            }

            mPreviousLevel = mCurrentLevel;
        }
        mCurrentLevel->Update();
    }
};
