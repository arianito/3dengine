#pragma once

struct __attribute__((aligned(16))) TRotator {
    float Pitch{0};
    float Yaw{0};
    float Roll{0};

    explicit inline TRotator() : Pitch(0), Yaw(0), Roll(0) {}

    explicit inline TRotator(const float &pitch, const float &yaw, const float &roll) : Pitch(pitch), Yaw(yaw), Roll(roll) {}

    explicit inline TRotator(const TVector &other) : Pitch(other.Y), Yaw(other.Z), Roll(other.X) {}

    // operator equal

    inline TRotator &operator=(const TRotator &other) {
        if (this != &other) {
            Pitch = other.Pitch;
            Yaw = other.Yaw;
            Roll = other.Roll;
        }
        return *this;
    }

    inline TRotator &operator=(TRotator &&other) noexcept {
        Pitch = other.Pitch;
        Yaw = other.Yaw;
        Roll = other.Roll;
        return *this;
    }

    inline TRotator &operator=(const TVector &other) {
        Pitch = other.Y;
        Yaw = other.Z;
        Roll = other.X;
        return *this;
    }


    inline TRotator &operator=(const float &other) {
        Pitch = other;
        Yaw = other;
        Roll = other;
        return *this;
    }


    inline float &operator[](int i) {
        assert(i >= 0 && i <= 2 && "TRotator: invalid index");
        if (i == 0)
            return Pitch;
        if (i == 1)
            return Yaw;
        if (i == 2)
            return Roll;
    }

};