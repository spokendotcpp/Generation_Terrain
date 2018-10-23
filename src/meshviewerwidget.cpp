#include <iostream>
#include "../include/meshviewerwidget.h"
#include "../include/axis.h"

MeshViewerWidget::MeshViewerWidget(QWidget* parent)
    :QGLWidget(parent)
{
    setMouseTracking(true);
    mouse_pressed = false;
    lap = HRClock::now();
}

/*
 * Destructor:
 *
 * Destroy every Buffers (Vertex Buffer Object: ARRAY_BUFFER, INDEX_BUFFER AND Vertex Array Object).
 * Free memory.
*/
MeshViewerWidget::~MeshViewerWidget()
{
    /*
    vao->destroy();
    vbo->destroy();
    ebo->destroy();

    delete vao;
    delete ebo;
    delete vbo;*/
    delete program;
}

/* Update the model matrix
 * -> no specific actions yet.
*/
void
MeshViewerWidget::update_model()
{
    model.setToIdentity();
}

/* Update the view matrix
 * i.e.: can be called after every user inputs (mouse, keyboard ...)
*/
void
MeshViewerWidget::update_view()
{
    view.setToIdentity();
    view.translate(position);
    view.rotate(angle.x(), 1.0f, 0.0f, 0.0f);
    view.rotate(angle.y(), 0.0f, 1.0f, 0.0f);
}

/* Update the projection matrix
 * -> used when changing window size.
*/
void
MeshViewerWidget::update_projection()
{
    projection.setToIdentity();
    projection.perspective(
        fov,
        static_cast<float>(size().width()/size().height()),
        zNear, zFar
    );
}

/* Matrix computation on CPU
 * -> must be call only when needed.
 * i.e.: before rendering.
*/
void
MeshViewerWidget::update_ModelViewProjection()
{   
    MVP = projection * view * model;
}

/* Load default values for the model matrix + update */
void
MeshViewerWidget::default_model()
{
    /* ?? */
    update_model();
}

/* Load default values for the view matrix + update */
void
MeshViewerWidget::default_view()
{
    angle = QVector3D(-45.0f, 0.0f, 0.0f);
    position = QVector3D(0.0f, 0.0f, -2.0f);
    update_view();
}

/* Load default values for the projection matrix + update */
void
MeshViewerWidget::default_projection()
{
    fov = 45.0f;
    zNear = 0.1f;
    zFar = 100.0f;
    update_projection();
}

/* Load every defaults */
void
MeshViewerWidget::default_positions()
{
    default_model();
    default_view();
    default_projection();
}

/* Setup our OpenGL context
 * Vertex Array Buffer,
 * Vertex Buffer Object,
 * Elements Buffer Object,
 * Shaders programs,
 * [ ... ]
*/
void
MeshViewerWidget::initializeGL()
{
    // Check if OpenGL context was successfully initialized
    if( !context()->isValid() ){
        std::cerr << "Failed to init OpenGL context" << std::endl;
        return;
    }

    /*
    GLfloat vertices[] = {
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5, 0.0f,
        -0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
    };

    GLuint indices[] = {
        0, 2, 1,
        2, 3, 1
    };*/


    // Création de notre AXIS
    Axis axis(0.0f, 0.0f, 0.0f, 5.0f);

    default_positions();

    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert.glsl");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag.glsl");
    program->link();
    program->bind();

    /*vao = new QOpenGLVertexArrayObject();
    vao->create();
    vao->bind();*/

    /*
    vbo = new QOpenGLBuffer();
    vbo->create();
    vbo->bind();
    vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo->allocate(vertices, sizeof(vertices));

    ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    ebo->create();
    ebo->bind();
    ebo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    ebo->allocate(indices, sizeof(indices));
    */

    program->enableAttributeArray(0);
    program->setAttributeBuffer(program->attributeLocation("position"), GL_FLOAT, 0, 3);

    // bind the location of our futur data into shader
    loc_MVP = program->uniformLocation("MVP");

    //vao->release();
    /*vbo->release();
    ebo->release();*/

    program->release();

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glClearColor(1.0f, 191.0f/255.0f, 179.0f/255.0f, 1.0f);
}

/* When window (this widget) is resized */
void
MeshViewerWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    update_projection();
    updateGL();
}

/* RENDER TIME */
void
MeshViewerWidget::paintGL()
{
    auto current_time = HRClock::now();
    std::cerr << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(current_time-lap).count() << " ms" << std::endl;
    lap = current_time;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* update the MVP matrix */
    update_ModelViewProjection();

    program->bind();
    // vao->bind();

    // Send matrix_world value to Vertex Shader
    program->setUniformValue(loc_MVP, MVP);

    // With Element Buffer Object :
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // vao->release();
    program->release();
}

/* When mouse is moving inside the widget */
void
MeshViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();

    if( mouse_pressed ){
        angle.setX(angle.x() + pos.y() - mouse.y());
        angle.setY(angle.y() + pos.x() - mouse.x());
        update_view();
        updateGL();
    }

    mouse = pos;
}

void
MeshViewerWidget::mousePressEvent(QMouseEvent* event)
{
    if( event->button() == Qt::MouseButton::LeftButton )
       mouse_pressed = true;
}

void
MeshViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if( event->button() == Qt::MouseButton::LeftButton )
        mouse_pressed = false;
}

void
MeshViewerWidget::wheelEvent(QWheelEvent* event)
{
    float step = 0.25f; // TODO? change this by something more efficient
    // i.e.: more the object go far, more we slow speed step.

    /* TODO:
     * Test if object goes outside of zNear & zFar positions.
     */

    if( event->delta() < 0 ) position.setZ(position.z()-step);  /* Wheel go down */
    else                     position.setZ(position.z()+step);  /* Wheel go up */

    update_view();
    updateGL();
}

void
MeshViewerWidget::handle_key_events(QKeyEvent* event)
{
    float step = 0.25f;
    int key = event->key();

    if( key == Qt::Key_Up )
        position.setZ(position.z()+step);
    else
    if( key == Qt::Key_Down )
        position.setZ(position.z()-step);
    else
    if( key == Qt::Key_Left )
        position.setX(position.x()+step);
    else
    if( key == Qt::Key_Right )
        position.setX(position.x()-step);
    else
    if( key == Qt::Key_V )
        default_positions();

    update_view();
    updateGL();
}
