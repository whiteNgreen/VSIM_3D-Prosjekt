#include "visualobject.h"

VisualObject::VisualObject()
{

}

VisualObject::~VisualObject()
{
    glDeleteVertexArrays( 1, &mVAO );
    glDeleteBuffers( 1, &mVBO );
}

void VisualObject::init()
{
    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );

    //Vertex Buffer Object to hold vertices - VBO
    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof( Vertex ), mVertices.data(), GL_STATIC_DRAW );

    // 1rst attribute buffer : vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(0 )  );          // array buffer offset
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : uvs
    glVertexAttribPointer(2, 2,  GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(6 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);

    glGenBuffers ( 1, &mEAB );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEAB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size()*sizeof(GLuint), mIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void VisualObject::draw()
{
    /* Om annet ikke er spesifisert så antar vi at den bruker 'plainshader' */
    m_shader->setUniformMatrix("mMatrix", mMatrix);

    glBindVertexArray( mVAO );
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}

Position2D VisualObject::getPosition2D()
{
    return std::pair<float, float>{mMatrix.column(3).x(), mMatrix.column(3).y()};
}

QVector2D VisualObject::getPositionVector2D()
{
    return {mMatrix.column(3).x(), mMatrix.column(3).y()};
}

QVector3D VisualObject::getPositionVector3D()
{
    return {mMatrix.column(3).x(), mMatrix.column(3).y(), mMatrix.column(3).z()};
}

