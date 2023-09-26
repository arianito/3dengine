#pragma once


struct __attribute__((aligned(16))) TRect {
    float MinX{0};
    float MinY{0};
    float MaxX{0};
    float MaxY{0};

    explicit inline TRect() : MinX(0), MinY(0), MaxX(0), MaxY(0) {}

    explicit inline TRect(const float &minX, const float &minY, const float &maxX, const float &maxY) : MinX(minX), MinY(minY), MaxX(maxX), MaxY(maxY) {}

    // operator equal

    inline TRect &operator=(const TRect &other) {
        if (this != &other) {
            MinX = other.MinX;
            MinY = other.MinY;
            MaxX = other.MaxX;
            MaxY = other.MaxY;
        }
        return *this;
    }

    inline TRect &operator=(TRect &&other) noexcept {
        MinX = other.MinX;
        MinY = other.MinY;
        MaxX = other.MaxX;
        MaxY = other.MaxY;
        return *this;
    }


    inline TRect &operator=(const float &other) {
        float ha = other / 2;
        MinX = -ha;
        MinY = -ha;
        MaxX = ha;
        MaxY = ha;
        return *this;
    }


    inline float &operator[](int i) {
        assert(i >= 0 && i <= 3 && "TRect: invalid index");
        if (i == 0)
            return MinX;
        if (i == 1)
            return MinY;
        if (i == 2)
            return MaxX;
        if (i == 3)
            return MaxY;
    }
};