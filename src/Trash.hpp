//
//struct Temp : public CLevel {
//
//    void Create() override {
//    }
//
//    void Update() override {
//
//        float t = gameTime->time;
//        const int latDivisions = 50.0f;
//        const int lonDivisions = 100.0f;
//
//        for (int lat = 0; lat <= latDivisions; lat++) {
//            const float theta = ((float)lat / latDivisions) * 180.0f;
//            for (int lon = 0; lon <= lonDivisions; lon++) {
//                const float phi = ((float)lon / lonDivisions) * 360.0f;
//
//                const float x = sind(theta) * cosd(phi);
//                const float y = cosd(theta);
//                const float z = sind(theta) * sind(phi);
//
//                float noise = noise_perlin_2d(theta + t * 8.0f, phi, 18.0f, 1);
//
//                Vec3 pos = Vec3{x, y, z} * 100.0f;
//                draw_point(pos, 2.0f + noise * 5.0f, color_lerp(color_green, color_red, (noise + 1) / 3.0f));
//
//            }
//        }
//
//
//
//    }
//};
