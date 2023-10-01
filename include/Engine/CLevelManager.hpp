#pragma once

#include <type_traits>

#include "mem/alloc.h"
#include "engine/Memory.hpp"
#include "data/TString.hpp"
#include "data/TFastMap.hpp"


typedef unsigned int CLevelId;

class CLevel {
protected:
public:
    virtual void Create() {};

    virtual void Update() {};

    virtual void Destroy() {};
};


namespace CLevelInternals {
    static inline CLevelId nextLevelId() {
        static CLevelId lastID{1};
        return lastID++;
    }

    template<class T>
    static inline CLevelId GetLevelTypeId() noexcept {
        static_assert(std::is_base_of_v<CLevel, T>, "T must be a base class of Level");
        static CLevelId id{nextLevelId()};
        return id;
    }
}

template<class TAlloc = FreeListMemory>
class CLevelManager {
private:
    TFastMap<CLevelId, CLevel *, TAlloc> mLevels;
    CLevel *mPreviousLevel = nullptr;
    CLevel *mCurrentLevel = nullptr;
public:
    inline ~CLevelManager() {
        if (mCurrentLevel)
            mCurrentLevel->Destroy();
        for (const auto &level: mLevels) {
            Free<FreeListMemory>(&level->value);
        }
    }

    template<typename T>
    inline void Add() {
        static_assert(std::is_base_of_v<CLevel, T>, "Level Manager: class must be type of Level");
        auto id = CLevelInternals::GetLevelTypeId<T>();
        if (mLevels.Contains(id)) return;
        CLevel *level = AllocNew<TAlloc, T>();
        mLevels.Set(id, level);
    }

    template<typename T>
    inline void Load() {
        auto id = CLevelInternals::GetLevelTypeId<T>();
        const auto level = mLevels.Get(id);
        if (level != nullptr)
            mCurrentLevel = *level;
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
