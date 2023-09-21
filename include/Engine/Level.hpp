#pragma once


typedef unsigned int LevelId;

class Level {
protected:
public:
    virtual void Create() {};

    virtual void Update() {};

    virtual void Destroy() {};
};

static inline LevelId nextLevelId() {
    static LevelId lastID{1};
    return lastID++;
}

template<class T>
static inline LevelId GetLevelTypeId() noexcept {
    static_assert(std::is_base_of_v<Level, T>, "T must be a base class of Level");
    static LevelId id{nextLevelId()};
    return id;
}