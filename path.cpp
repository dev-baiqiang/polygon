// Copyright (c) 2018 BaiQiang. All rights reserved.


#include "path.h"

static float min(float a, float b) { return a < b ? a : b; }

static float max(float a, float b) { return a > b ? a : b; }

polygon::Path polygon::Path::addPoint(Point p) {
    this->mPs.push_back(p);
    return *this;
}

polygon::Path polygon::Path::build() {
    assert(this->mPs.size() > 2);
    if (this->mStyle == polygon::Path::Style::kFill) {
        return this->buildFill();
    } else {
        return this->buildStroke();
    }
}

polygon::Path polygon::Path::buildFill() {
    fillOffset = mPs.size();
    float v0 = 1e6f, v1 = 1e6f, v2 = -1e6f, v3 = -1e6f;

    for (int i = 0; i < fillOffset; ++i) {
        v0 = min(v0, mPs[i].x);
        v1 = min(v1, mPs[i].y);
        v2 = max(v2, mPs[i].x);
        v3 = max(v3, mPs[i].y);
    }

    this->vs = mPs;
    this->vs.emplace_back(Point(v2, v3));
    this->vs.emplace_back(Point(v2, v1));
    this->vs.emplace_back(Point(v0, v3));
    this->vs.emplace_back(Point(v0, v1));

    return *this;
}

polygon::Path polygon::Path::buildStroke() {

    bool sharp = false;
    bool lastSharp = false;
    float halfWidth = this->mStrokeWidth / 2;
    unsigned int i;

    if (!this->mClosed) {
        this->is.push_back(0);
        this->is.push_back(1);
        this->is.push_back(3);
        this->is.push_back(1);
        this->is.push_back(2);
        this->is.push_back(3);
    } else {
        this->mPs.push_back(this->mPs[0]);
        this->mPs.push_back(this->mPs[1]);
        this->mPs.push_back(this->mPs[2]);
    }

    int size = this->mPs.size();

    for (i = 0; i < size - 2; ++i) {
        Point current = this->mPs[i];
        Point next1 = this->mPs[i + 1];
        Point next2 = this->mPs[i + 2];

        glm::vec2 currentLine(next1 - current);
        auto currentNormal = glm::normalize(glm::vec2(-currentLine.y, currentLine.x));
        if (i == 0) {
            auto p0 = current + halfWidth * currentNormal;
            auto p1 = current - halfWidth * currentNormal;
            this->vs.push_back({p0.x, p0.y});
            this->vs.push_back({p1.x, p1.y});
        }

        glm::vec2 tangent = glm::normalize(glm::normalize(next2 - next1) + glm::normalize(next1 - current));
        glm::vec2 miter = glm::vec2(-tangent.y, tangent.x);
        float mLen = halfWidth / glm::dot(miter, currentNormal);
        auto m1 = next1 - miter * mLen;
        auto m2 = next1 + miter * mLen;
        glm::vec2 nextLine(next2 - next1);
        auto nextNormal = glm::normalize(glm::vec2(-nextLine.y, nextLine.x));
        float tLen1 = halfWidth / glm::dot(tangent, currentNormal);
        float tLen2 = halfWidth / glm::dot(tangent, nextNormal);
        auto t1 = next1 - tangent * tLen1;
        auto t2 = next1 - tangent * tLen2;
        sharp = glm::dot(miter, currentNormal) > asin(0.25) * 2;

        if (sharp) {
            this->vs.push_back({m1.x, m1.y});
            this->vs.push_back({m2.x, m2.y});
        } else {
            this->vs.push_back({t1.x, t1.y});
            this->vs.push_back({t2.x, t2.y});
        }

        if (i + 2 == size - 1) {
            auto last1 = next2 + halfWidth * nextNormal;
            auto last2 = next2 - halfWidth * nextNormal;
            this->vs.push_back({last1.x, last1.y});
            this->vs.push_back({last2.x, last2.y});
        }
        if (i == 0) {
            lastSharp = sharp;
            continue;
        }

        if (lastSharp) {
            this->is.push_back(i * 2);
            this->is.push_back(i * 2 + 1);
            this->is.push_back(i * 2 + 3);
            this->is.push_back(i * 2);
            this->is.push_back(i * 2 + 2);
            this->is.push_back(i * 2 + 3);
        } else {
            this->is.push_back(i * 2);
            this->is.push_back(i * 2 + 1);
            this->is.push_back(i * 2 + 2);
            this->is.push_back(i * 2);
            this->is.push_back(i * 2 + 2);
            this->is.push_back(i * 2 + 3);
        }
        lastSharp = sharp;
    }

    if (this->mClosed == true) {
        return *this;
    }
    // Add last triangle
    if (sharp) {
        this->is.push_back(i * 2 + 1);
        this->is.push_back(i * 2);
        this->is.push_back(i * 2 + 3);
        this->is.push_back(i * 2 + 1);
        this->is.push_back(i * 2 + 2);
        this->is.push_back(i * 2 + 3);
    } else {
        this->is.push_back(i * 2);
        this->is.push_back(i * 2 + 1);
        this->is.push_back(i * 2 + 3);
        this->is.push_back(i * 2);
        this->is.push_back(i * 2 + 2);
        this->is.push_back(i * 2 + 3);
    }
    return *this;
}
