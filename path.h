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
        bool fill;
        float strokeWidth;

    public:
        enum Style {
            Stroke, Fill
        };

        explicit Path(std::vector<Point> ps, int style = Path::Style::Stroke, float strokeWidth = 10.0f, bool fill = false) {
            this->ps = std::move(ps);
            this->style = style;
            this->strokeWidth = strokeWidth;
            this->fill = fill;
        };

        Path addPoint(Point);

        std::vector<Vertex> vs;
        std::vector<GLuint> is;

        Path build();

    };
}

#endif //POLYGON_PATH_H
