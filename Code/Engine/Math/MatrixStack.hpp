#pragma once

#include <vector>
#include "Engine/Math/Matrix4.hpp"

class MatrixStack {
public:
	MatrixStack();
	~MatrixStack();

    void push(const Matrix4& mat);
    void push_direct(const Matrix4& mat);
    void pop();
    const Matrix4& top() const;
    Matrix4& top();

protected:
private:
    Matrix4 _top;
    std::vector<Matrix4> _stack;
};