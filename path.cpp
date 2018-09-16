// Copyright (c) 2018 BaiQiang. All rights reserved.


#include "path.h"

polygon::Path polygon::Path::addPoint(Point p) {
    this->ps.push_back(p);
    return *this;
}

polygon::Path polygon::Path::build() {
    assert(this->ps.size() > 2);
    if (this->style == polygon::Path::Style::kFill) {
        return this->buildFill();
    } else {
        return this->buildStroke();
    }
}

polygon::Path polygon::Path::buildFill() {
    for (auto p : this->ps) {
        this->vs.push_back({p.x, p.y});
    }
    return *this;
}

polygon::Path polygon::Path::buildStroke() {

    bool sharp = false;
    bool lastSharp = false;
    float halfWidth = this->strokeWidth / 2;
    unsigned int i;

    if (!this->closed) {
        this->is.push_back(0);
        this->is.push_back(1);
        this->is.push_back(3);
        this->is.push_back(1);
        this->is.push_back(2);
        this->is.push_back(3);
    } else {
        this->ps.push_back(this->ps[0]);
        this->ps.push_back(this->ps[1]);
        this->ps.push_back(this->ps[2]);
    }

    int size = this->ps.size();

    for (i = 0; i < size - 2; ++i) {
        Point current = this->ps[i];
        Point next1 = this->ps[i + 1];
        Point next2 = this->ps[i + 2];

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

    if (this->closed == true) {
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
