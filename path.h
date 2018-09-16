// Copyright (c) 2018 BaiQiang. All rights reserved.

#ifndef POLYGON_PATH_H
#define POLYGON_PATH_H

#include <iostream>
#include <algorithm>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>

#include <glad/glad.h>

namespace polygon {

    typedef glm::vec2 Point;

    struct Vertex {
        float x;
        float y;
    };

    class Path {
    private:
        std::vector<Point> ps;
        int style;
        float strokeWidth;
        bool closed;

        Path buildFill();

        Path buildStroke();

    public:
        enum Style {
            kFill,
            kStroke
        };

        explicit Path(std::vector<Point> ps, int style = Path::kStroke, float strokeWidth = 10.0f) {
            this->ps = std::move(ps);
            this->style = style;
            this->strokeWidth = strokeWidth;
            this->closed = false;
        };

        Path addPoint(Point);

        std::vector<Vertex> vs;
        std::vector<GLuint> is;

        Path build();

        inline void close() {
            this->closed = true;
        };

        inline int getStyle() {
            return this->style;
        }

    };
}

#endif //POLYGON_PATH_H
