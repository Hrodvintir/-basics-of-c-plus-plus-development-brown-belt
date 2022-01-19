#include "Common.h"

#include <iostream>

using namespace std;

// Этот файл сдаётся на проверку
// Здесь напишите реализацию необходимых классов-потомков `IShape`

class Rectangle : public IShape {
public:
    ~Rectangle() = default;

    unique_ptr<IShape> Clone() const override {
        auto clone = std::make_unique<Rectangle>();
        clone->SetSize(size_);
        clone->SetPosition(position_);
        clone->SetTexture(texture_);
        return clone;
    }

    void SetPosition(Point point) override {
        position_ = point;
    }

    Point GetPosition() const override {
        return position_;
    }

    void SetSize(Size size) override {
        size_ = size;
    }

    Size GetSize() const override {
        return size_;
    }

    void SetTexture(std::shared_ptr<ITexture> ptr) override {
        texture_ = ptr;
    }

    ITexture *GetTexture() const override {
        return texture_.get();
    }

    void Draw(Image &image) const override {
        for (size_t i = position_.y; i < image.size(); ++i) {
            if (i - position_.y < size_.height) {
                for (size_t j = position_.x; j < image[i].size(); ++j) {
                    if (j - position_.x < size_.width) {
                        if (texture_ == nullptr) {
                            image[i][j] = '.';
                        } else {
                            if ((i >= position_.y && i < position_.y + texture_->GetSize().height)
                                && (j >= position_.x && j < position_.x + texture_->GetSize().width)) {
                                image[i][j] = texture_->GetImage()[i - position_.y][j - position_.x];
                            } else {
                                image[i][j] = '.';
                            }
                        }
                    }
                }
            }
        }
    }

private:
    Size size_;
    Point position_;
    std::shared_ptr<ITexture> texture_;
};

class Ellipse : public IShape {
public:
    ~Ellipse() = default;

    unique_ptr<IShape> Clone() const override {
        auto clone = std::make_unique<Ellipse>();
        clone->SetSize(size_);
        clone->SetPosition(position_);
        clone->SetTexture(texture_);
        return clone;
    }

    void SetPosition(Point point) override {
        position_ = point;
    }

    Point GetPosition() const override {
        return position_;
    }

    void SetSize(Size size) override {
        size_ = size;
    }

    Size GetSize() const override {
        return size_;
    }

    void SetTexture(std::shared_ptr<ITexture> ptr) override {
        texture_ = ptr;
    }

    ITexture *GetTexture() const override {
        return texture_.get();
    }

    void Draw(Image &image) const override {
        for (auto i = position_.y; i < image.size(); ++i) {
            if (i - position_.y < size_.height) {
                for (auto j = position_.x; j < image[i].size(); ++j) {
                    if (j - position_.x < size_.width) {
                        if (texture_ == nullptr) {
                            image[i][j] = '.';
                        } else {
                            if (IsPointInEllipse({j - position_.x, i - position_.y}, size_)) {
                                if ((i >= position_.y && i < position_.y + texture_->GetSize().height)
                                    && (j >= position_.x && j < position_.x + texture_->GetSize().width)) {
                                    image[i][j] = texture_->GetImage()[i - position_.y][j - position_.x];
                                } else {
                                    image[i][j] = '.';
                                }
                            }
                        }
                    }
                }
            }
        }
    }

private:
    Size size_;
    Point position_;
    std::shared_ptr<ITexture> texture_;
};

// Напишите реализацию функции
unique_ptr<IShape> MakeShape(ShapeType shape_type) {
    if (shape_type == ShapeType::Rectangle) {
        return make_unique<Rectangle>();
    } else if (shape_type == ShapeType::Ellipse) {
        return make_unique<Ellipse>();
    }
}
