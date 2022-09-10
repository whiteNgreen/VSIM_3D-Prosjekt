#include "octahedronball.h"
#include "logger.h"

OctahedronBall::OctahedronBall(int recursions, float radius)
    : m_rekursjoner(recursions), m_indeks(0)
{
    mLogger = Logger::getInstance();

    mVertices.reserve(3*8*pow(4, m_rekursjoner));

    size = radius;
    /* Size of ball */
    QVector3D v0{    0,     0,   size};
    QVector3D v1{ size,     0,      0};
    QVector3D v2{    0,  size,      0};
    QVector3D v3{-size,     0,      0};
    QVector3D v4{    0, -size,      0};
    QVector3D v5{    0,     0,  -size};

    /* Subdivide ball */
    subDivide(v0, v1, v2, m_rekursjoner);
    subDivide(v0, v2, v3, m_rekursjoner);
    subDivide(v0, v3, v4, m_rekursjoner);
    subDivide(v0, v4, v1, m_rekursjoner);
    subDivide(v5, v2, v1, m_rekursjoner);
    subDivide(v5, v3, v2, m_rekursjoner);
    subDivide(v5, v4, v3, m_rekursjoner);
    subDivide(v5, v1, v4, m_rekursjoner);
}

void OctahedronBall::CalculatePhysics(float DeltaTime)
{
    Position_PreviousFrame = getPositionVector3D();

    QVector3D ChangeInPosition;
    {
        Velocity.setZ(Velocity.z() + (DeltaTime * GravityConstant));

        Speed = Velocity.length();
        mLogger->logText("Speed: " + std::to_string(Speed));

        ChangeInPosition = Velocity;
    }
    Estimated_Position_NextFrame = Position + ChangeInPosition;

//    if (Estimated_Position_NextFrame.z() < 0.f)
//    {

//    }

    /* If ball is on the ground */
    if (Position.z() + ChangeInPosition.z() < 0.f)
    {
        float difference = 0 - (Position.z() + ChangeInPosition.z());
        ChangeInPosition.setZ(ChangeInPosition.z() + difference);

        Velocity.setZ(0.f);
    }

    mMatrix.translate(ChangeInPosition);
    Position = getPositionVector3D();
}

void OctahedronBall::subDivide(const QVector3D &a, const QVector3D &b, const QVector3D &c, int n)
{
    if (n>0)
    {
        QVector3D v1 = a+b; v1.normalize(); v1 *= size;
        QVector3D v2 = a+c; v2.normalize(); v2 *= size;
        QVector3D v3 = c+b; v3.normalize(); v3 *= size;
        subDivide(a, v1, v2, n-1);
        subDivide(c, v2, v3, n-1);
        subDivide(b, v3, v1, n-1);
        subDivide(v3, v2, v1, n-1);
    }
    else
    {
        lagTriangel(a,b,c);
    }
}

void OctahedronBall::lagTriangel(const QVector3D &v1, const QVector3D &v2, const QVector3D &v3)
{
    QVector3D color{20, 20, 200};
    mVertices.push_back(Vertex(v1, color));
    mVertices.push_back(Vertex(v2, color));
    mVertices.push_back(Vertex(v3, color));

    int m = mVertices.size();
    mIndices.push_back(m-3);
    mIndices.push_back(m-2);
    mIndices.push_back(m-1);
}

void OctahedronBall::init()
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

void OctahedronBall::draw()
{
    m_shader->setUniformMatrix("mMatrix", mMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindVertexArray( mVAO );
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
//    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());

        glBindVertexArray(0);
}

void OctahedronBall::draw(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix)
{
    glUseProgram(m_shader->getProgram());
    m_shader->setUniformMatrix("mMatrix", mMatrix);
    m_shader->setUniformMatrix("pMatrix", projectionMatrix);
    m_shader->setUniformMatrix("vMatrix", viewMatrix);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
//    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    glBindVertexArray(0);
}
