#include "my_opengl_widget.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    vertex_buffer(QOpenGLBuffer::VertexBuffer),
    color_buffer(QOpenGLBuffer::VertexBuffer),
    index_buffer(QOpenGLBuffer::IndexBuffer)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    setFormat(format);

    connect(&timer, &QTimer::timeout, this, &MyOpenGLWidget::onTimer);
    timer.start(60);
}

void MyOpenGLWidget::initializeGL() {
    auto *gl = context()->functions();

    gl->glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glEnable(GL_MULTISAMPLE);

    initView();
    initProgram();
}

void MyOpenGLWidget::initProgram() {
    // Vertices of the cube.
    const std::vector<QVector3D> vertices = {
        // front
        QVector3D(-1.0f, -1.0f,  1.0f),
        QVector3D(1.0f, -1.0f,  1.0f),
        QVector3D(1.0f,  1.0f,  1.0f),
        QVector3D(-1.0f,  1.0f,  1.0f),
        // back
        QVector3D(-1.0f, -1.0f, -1.0f),
        QVector3D(1.0f, -1.0f, -1.0f),
        QVector3D(1.0f,  1.0f, -1.0f),
        QVector3D(-1.0f,  1.0f, -1.0f)
    };

    // Vertex colors.
    const std::vector<QVector3D> colors = {
        // front colors
        QVector3D(1.0f, 0.0f, 0.0f),
        QVector3D(0.0f, 1.0f, 0.0f),
        QVector3D(0.0f, 0.0f, 1.0f),
        QVector3D(1.0f, 1.0f, 1.0f),
        // back colors
        QVector3D(1.0f, 0.0f, 0.0f),
        QVector3D(0.0f, 1.0f, 0.0f),
        QVector3D(0.0f, 0.0f, 1.0f),
        QVector3D(1.0f, 1.0f, 1.0f)
    };

    // Vertex indices: each face of the cube is two triangles.
    const std::vector<unsigned int> indices = {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
    };

    num_of_indices = static_cast<int>(indices.size());

    program = std::make_shared<QOpenGLShaderProgram>();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/basic.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/basic.frag");
    program->link();
    program->bind();

    vao.create();
    vao.bind();

    // Load vertex coordinates.
    vertex_buffer.create();
    vertex_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertex_buffer.bind();
    vertex_buffer.allocate(vertices.data(), static_cast<int>(vertices.size()*sizeof(QVector3D)));

    const auto v_loc = program->attributeLocation("vertex");
    program->enableAttributeArray(v_loc);
    program->setAttributeBuffer(v_loc, GL_FLOAT, 0, 3);

    // Load vertex colors.
    color_buffer.create();
    color_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    color_buffer.bind();
    color_buffer.allocate(colors.data(), static_cast<int>(colors.size()*sizeof(QVector3D)));

    const auto c_loc = program->attributeLocation("color");
    program->enableAttributeArray(c_loc);
    program->setAttributeBuffer(c_loc, GL_FLOAT, 0, 3);

    // Load indices.
    index_buffer.create();
    index_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    index_buffer.bind();
    index_buffer.allocate(indices.data(), static_cast<int>(indices.size()*sizeof(indices)));

    vao.release();
    program->release();
}

void MyOpenGLWidget::resizeGL(int width, int height) {
    auto *gl = context()->functions();

    gl->glViewport(0, 0, width, height);

    initView();
}

void MyOpenGLWidget::initView() {
    model_matrix.setToIdentity();

    view_matrix.setToIdentity();
    const auto eye = QVector3D(3.0f, 3.0f, 3.0f);
    const auto center = QVector3D(0.0f, 0.0f, 0.0f);
    const auto up = QVector3D(0.0f, 1.0f, 0.0f);
    view_matrix.lookAt(eye, center, up);

    projection_matrix.setToIdentity();
    const auto angle = 45.0f;
    const auto aspect = float(width())/height();
    const auto near_plane = 0.01f;
    const auto far_plane = 100.0f;
    projection_matrix.perspective(angle, aspect, near_plane, far_plane);
}

void MyOpenGLWidget::paintGL() {
    auto *gl = context()->functions();

    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!program) {
        return;
    }

    program->bind();

    // Apply rotation.
    QMatrix4x4 rotate;
    rotate.rotate(rotation_angle, QVector3D(0.0f, 1.0f, 0.0f));
    const auto mvp_loc = program->uniformLocation("MVP");
    program->setUniformValue(mvp_loc, projection_matrix*view_matrix*rotate*model_matrix);

    // Draw the cube with triangles.
    vao.bind();
    gl->glDrawElements(GL_TRIANGLES, num_of_indices, GL_UNSIGNED_INT, nullptr);
    vao.release();

    program->release();
}

void MyOpenGLWidget::onTimer() {
    rotation_angle += 1.0f;
    update();
}
