#ifndef VISUALPOINT_H
#define VISUALPOINT_H

#include "visualobject.h"

class VisualPoint : public VisualObject
{
public:
    VisualPoint(Shader* shader, const QVector3D& Location, bool ShowPoint, float PointSize = 5.f)
    {
        setShader(shader);

        bShowPoint = ShowPoint;

        QVector3D color { 1, 1, 1 };
        mVertices.push_back(Vertex{Location, color});

        mPointSize = PointSize;

        init();
    }
    ~VisualPoint(){}

    bool bShowPoint{ true };
    float mPointSize{ 1.f };

    void draw(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix) override
    {
        if (bShowPoint)
        {
            glUseProgram(getShader()->getProgram());
            getShader()->setUniformMatrix("mMatrix", mMatrix);
            getShader()->setUniformMatrix("pMatrix", projectionMatrix);
            getShader()->setUniformMatrix("vMatrix", viewMatrix);

            glBindVertexArray( mVAO );
            glDrawArrays(GL_POINTS, 0, 1);
            glPointSize(mPointSize);

            glBindVertexArray(0);
        }
    }
};

#endif // VISUALPOINT_H
