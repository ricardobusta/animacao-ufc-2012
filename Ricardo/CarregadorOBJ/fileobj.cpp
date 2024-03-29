#include "fileobj.h"

#include <QtOpenGL>

FileObj::FileObj()
{
}

void FileObj::DrawObject(bool, bool)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glBegin(GL_TRIANGLES);
    for(int i=0;i<face.size();i++){
        FileObjFace thisface = face[i];

        for(int j=0;j<3;j++){
            glTexCoord2f(texturecoord[thisface.textureID[j]].x(),texturecoord[thisface.textureID[j]].y());
            glNormal3f(normal[thisface.normalID[j]].x(),normal[thisface.normalID[j]].y(),normal[thisface.normalID[j]].z());
            glVertex3f(vertex[thisface.vertexID[j]].x(),vertex[thisface.vertexID[j]].y(),vertex[thisface.vertexID[j]].z());
        }
    }
    glEnd();
    glPopAttrib();
}

#include <iostream>
using namespace std;

void FileObj::loadFile(QString filename)
{

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);

    QVector3D vecv(0,0,0);
    vertex.push_back(vecv);
    QVector2D vect(0,0);
    texturecoord.push_back(vect);
    QVector3D vecn(0,0,0);
    normal.push_back(vecn);

    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList token = line.split(' ');

        if(token.at(0) == "v" and token.size() == 4){
            QVector3D vec(token.at(1).toFloat(),token.at(2).toFloat(),token.at(3).toFloat());
            vertex.push_back(vec);
            cout << "v " << vec.x() << " " << vec.y() << " " << vec.z() << endl;
        }else if(token.at(0) == "vt" and token.size() == 3){
            QVector2D vec(token.at(1).toFloat(),token.at(2).toFloat());
            texturecoord.push_back(vec);
            cout << "vt " << vec.x() << " " << vec.y() << endl;
        }else if(token.at(0) == "vn" and token.size() == 4){
            QVector3D vec(token.at(1).toFloat(),token.at(2).toFloat(),token.at(3).toFloat());
            normal.push_back(vec);
            cout << "vn " << vec.x() << " " << vec.y() << " " << vec.z() << endl;
        }else if(token.at(0) == "f" and token.size() == 4){
            FileObjFace f;

            QStringList v0 = token.at(1).split('/');
            QStringList v1 = token.at(2).split('/');
            QStringList v2 = token.at(3).split('/');

            if(v1.size() == 3 and v2.size() == 3 and v0.size() == 3){
                f.vertexID[0] = v0.at(0).toInt();
                f.textureID[0] = v0.at(1).toInt();
                f.normalID[0] = v0.at(2).toInt();

                f.vertexID[1] = v1.at(0).toInt();
                f.textureID[1] = v1.at(1).toInt();
                f.normalID[1] = v1.at(2).toInt();

                f.vertexID[2] = v2.at(0).toInt();
                f.textureID[2] = v2.at(1).toInt();
                f.normalID[2] = v2.at(2).toInt();
            }else{
                f.vertexID[0] = 0;
                f.textureID[0] = 0;
                f.normalID[0] = 0;

                f.vertexID[1] = 0;
                f.textureID[1] = 0;
                f.normalID[1] = 0;

                f.vertexID[2] = 0;
                f.textureID[2] = 0;
                f.normalID[2] = 0;
            }
            cout << "f";
            for(int i=0;i<3;i++){
                cout << " " << f.vertexID[i] << "/" << f.textureID[i] << "/" << f.normalID[i] ;
            }
            cout << endl;
            face.push_back(f);
        }else{

        }
    }

    cout << vertex.size() << " " << texturecoord.size() << " " << normal.size() << " " << face.size() << endl;
    file.close();

}
