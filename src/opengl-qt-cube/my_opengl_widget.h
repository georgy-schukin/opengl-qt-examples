#pragma once

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QTimer>
#include <memory>

class QOpenGLShaderProgram;

class MyOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit MyOpenGLWidget(QWidget *parent=nullptr);

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int width, int height) override;
    virtual void paintGL() override;

private:
    void initProgram();
    void initView();
    void onTimer();

private:
    std::shared_ptr<QOpenGLShaderProgram> program;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vertex_buffer, color_buffer, index_buffer;
    QMatrix4x4 model_matrix, view_matrix, projection_matrix;
    float rotation_angle {0.0f};
    QTimer timer;
    int num_of_indices {0};
};

