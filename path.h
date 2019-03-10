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

    class Path {
    private:
        std::vector<Point> mPs;
        int mStyle;
        float mStrokeWidth;
        bool mClosed;

        Path buildFill();

        Path buildStroke();

    public:
        enum Style {
            kFill,
            kStroke
        };

        explicit Path(std::vector<Point> &ps, int style = Path::kStroke, float strokeWidth = 10.0f) :
                mPs(ps), mStyle(style), mStrokeWidth(strokeWidth), mClosed(false) {}

        Path addPoint(Point);

        std::vector<Point> vs;
        std::vector<GLuint> is;

        Path build();

        off_t fillOffset;

        inline void close() {
            this->mClosed = true;
        };

        inline int style() {
            return this->mStyle;
        }

    };
}

#endif //POLYGON_PATH_H
