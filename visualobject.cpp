#include "visualobject.h"

VisualObject::VisualObject()
{

}

VisualObject::~VisualObject()
{
    glDeleteVertexArrays( 1, &mVAO );
    glDeleteBuffers( 1, &mVBO );
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


