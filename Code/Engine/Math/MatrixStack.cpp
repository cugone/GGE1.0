#include "Engine/Math/MatrixStack.hpp"

MatrixStack::MatrixStack()
    : _top{}
    , _stack{}
{
    /* DO NOTHING */
}

MatrixStack::~MatrixStack() {
    _stack.clear();
}

void MatrixStack::push(const Matrix4& mat) {
    _stack.push_back(_top);
    _top = mat * _top; //RHS multiply
}

void MatrixStack::push_direct(const Matrix4& mat) {
    _stack.push_back(_top);
    _top = mat;
}

void MatrixStack::pop() {
    _top = _stack.back();
    _stack.pop_back();
}

const Matrix4& MatrixStack::top() const {
    return _top;
}

Matrix4& MatrixStack::top() {
    return const_cast<Matrix4&>(static_cast<const MatrixStack&>(*this).top());
}