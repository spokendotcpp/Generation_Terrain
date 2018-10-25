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
    axis_destroy();
    if( program != nullptr ){
        delete program;
        program = nullptr;
    }
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
    angle = QVector3D(25.0f, 25.0f, 0.0f);
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

void
MeshViewerWidget::axis_init()
{
    Axis* axis = new Axis(0.0f, 0.0f, 0.0f, 1.0f);

    vao_axis = new QOpenGLVertexArrayObject();
    vao_axis->create();
    vao_axis->bind();

    ibo_axis = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    ibo_axis->create();
    ibo_axis->bind();
    ibo_axis->setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo_axis->allocate(axis->indices(), 6 * sizeof(unsigned int));

    vbo_axis = new QOpenGLBuffer();
    vbo_axis->create();
    vbo_axis->bind();
    vbo_axis->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_axis->allocate(axis->vertices(), 18 * sizeof(float));

    program->enableAttributeArray(program->attributeLocation("position"));
    program->setAttributeBuffer(program->attributeLocation("position"), GL_FLOAT, 0, 3);

    float colors [18] = {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
    };

    vbo_color_axis = new QOpenGLBuffer();
    vbo_color_axis->create();
    vbo_color_axis->bind();
    vbo_color_axis->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_color_axis->allocate(colors, sizeof(float) * 18);

    program->enableAttributeArray(program->attributeLocation("inColor"));
    program->setAttributeBuffer(program->attributeLocation("inColor"), GL_FLOAT, 0, 3);

    vao_axis->release();
    ibo_axis->release();
    vbo_axis->release();
    vbo_color_axis->release();

    delete axis;
}

void
MeshViewerWidget::axis_render()
{
    vao_axis->bind();
    glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, nullptr);
    vao_axis->release();
}

void
MeshViewerWidget::axis_destroy()
{
    if( vao_axis != nullptr ){
        vao_axis->destroy();
        delete vao_axis;
        vao_axis = nullptr;
    }

    if( ibo_axis != nullptr ){
        vbo_axis->destroy();
        delete ibo_axis;
        ibo_axis = nullptr;
    }

    if( vbo_axis != nullptr ){
        vbo_axis->destroy();
        delete vbo_axis;
        vbo_axis = nullptr;
    }
}

int nb_indices = 0;

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

    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "../shaders/simple.vert.glsl");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "../shaders/simple.frag.glsl");
    program->link();
    program->bind();

    loc_MVP = program->uniformLocation("MVP");

    axis_init();
    program->release();


    /* LOAD MESH */
    MyMesh mesh;
    OpenMesh::IO::read_mesh(mesh, "../mesh_files/bunnyLowPoly.obj");
    mesh.request_vertex_colors();

    GLuint* IndiceArray = new GLuint[mesh.n_faces() * 3];
    MyMesh::ConstFaceVertexIter fvIt;

    // Copy Indices
    size_t i = 0;
    size_t j = 0;
    for( const auto& fIt: mesh.faces() ){
        fvIt = mesh.cfv_iter(fIt);
        j+=3;
        while( i < j ){
            IndiceArray[i++] = static_cast<GLuint>(fvIt->idx());
            ++fvIt;
        }
    }

    nb_indices = j;

    // Copy points
    i = 0;
    GLfloat* vertices = new GLfloat[mesh.n_vertices() * 3];
    for( const auto& vIt: mesh.vertices() ){
        MyMesh::Point p = mesh.point(vIt);
        for(j=0; j < 3; ++j, ++i){
            vertices[i] = p[j];
            //std::cerr << vertices[i] << std::endl;
        }
    }

    program->bind();

    vao_mesh = new QOpenGLVertexArrayObject();
    vao_mesh->create();
    vao_mesh->bind();

    QOpenGLBuffer* vbo_mesh = new QOpenGLBuffer();
    vbo_mesh->create();
    vbo_mesh->bind();
    vbo_mesh->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_mesh->allocate(vertices, sizeof(GLfloat) * mesh.n_vertices()*3);

    QOpenGLBuffer* ibo_mesh = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    ibo_mesh->create();
    ibo_mesh->bind();
    ibo_mesh->setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo_mesh->allocate(IndiceArray, sizeof(GLuint) * mesh.n_faces()*3);

    program->enableAttributeArray(program->attributeLocation("position"));
    program->setAttributeBuffer(program->attributeLocation("position"), GL_FLOAT, 0, 3);

    vao_mesh->release();
    vbo_mesh->release();
    ibo_mesh->release();
    program->release();

    delete [] vertices;
    delete [] IndiceArray;

    /* ------------------- */

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glClearColor(1.0f, 191.0f/255.0f, 179.0f/255.0f, 1.0f);
    default_positions();
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
    program->setUniformValue(loc_MVP, MVP);// Send matrix_world value to Vertex Shader

    axis_render();

    vao_mesh->bind();
    glDrawElements(GL_TRIANGLES, nb_indices, GL_UNSIGNED_INT, nullptr);
    vao_mesh->release();

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
