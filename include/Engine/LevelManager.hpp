#pragma once

#include <type_traits>

#include "mem/alloc.h"
#include "engine/Memory.hpp"
#include "data/String.hpp"
#include "data/ProbeHashTable.hpp"
#include "engine/Level.hpp"

template<class TAlloc = FreeListMemory>
class LevelManager {
private:
    ProbeHashTable<String<>, Level *, TAlloc> mLevels;
    Level *mPreviousLevel = nullptr;
    Level *mCurrentLevel = nullptr;
public:
    inline ~LevelManager() {
        if (mCurrentLevel)
            mCurrentLevel->Destroy();
        for (const auto &level: mLevels) {
            Free<FreeListMemory>((void **) &level.second);
        }
    }

    template<typename T>
    inline void Add(const String<> &name) {
        static_assert(std::is_base_of_v<Level, T>, "Level Manager: class must be type of Level");
        if (mLevels.Contains(name)) return;
        Level *level = AllocNew<TAlloc, T>();
        mLevels.Set(name, level);
    }

    inline void Load(const String<> &name) {
        if (!mLevels.Contains(name)) return;
        mCurrentLevel = mLevels[name];
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
