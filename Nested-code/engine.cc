#include "easy_image.h"
#include "ini_configuration.h"
#include "Lines2D.h"
#include "l_parser.h"
#include "vector3d.h"
#include "ThreeDfigures.h"
#include "Lichamen3D.h"
#include "ZBuffer.h"
#include "Light.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cmath>
#include <math.h>
#include <random>
#include <time.h>
#include <algorithm>

void backgroundcolorer(img::EasyImage &image, int x, int y, img::Color &color){
    for(int tempX=x-1;tempX!=-1;tempX--){
        image.draw_line(tempX,0,tempX,y-1,color);
    }
}

void draw2DLines(const Lines2D &Lines, img::EasyImage& image){
    for(std::vector<Line2D>::const_iterator it=Lines.begin();it!=Lines.end();it++){
        int x0=round(it->p1.x);
        int y0=round(it->p1.y);
        int x1=round(it->p2.x);
        int y1=round(it->p2.y);
        image.draw_line(x0,y0,x1,y1,it->color);
    }
}

img::EasyImage Lsystem(const ini::Configuration &configuration){
    srand(time(NULL));
    ini::Section general = configuration.operator[]("General");
    ini::Entry size = general.operator[]("size");
    ini::Entry backgroundcolor = general.operator[]("backgroundcolor");
    ini::Section _2DLSystem = configuration.operator[]("2DLSystem");
    ini::Entry inputfile = _2DLSystem.operator[]("inputfile");
    ini::Entry color = _2DLSystem.operator[]("color");
    int sizeParsed;
    ini::DoubleTuple backgroundcolorParsed;
    std::string inputfileParsed;
    ini::DoubleTuple colorParsed;
    size.as_int_if_exists(sizeParsed);
    backgroundcolor.as_double_tuple_if_exists(backgroundcolorParsed);
    inputfile.as_string_if_exists(inputfileParsed);
    color.as_double_tuple_if_exists(colorParsed);
    std::ifstream IFP;
    IFP.open(inputfileParsed);
    LParser::LSystem2D LSystem1 = LParser::LSystem2D(IFP);
    IFP.close();
    const std::set<char> alphabet = LSystem1.get_alphabet();
    const std::string initiator = LSystem1.get_initiator();
    double angle = LSystem1.get_angle();
    const double startingAngle = LSystem1.get_starting_angle();
    int nrIterations = LSystem1.get_nr_iterations();
    std::vector<std::pair<char,std::string>> stochasts = LSystem1.get_stochasts();
    double pi= atan(1)*4;
    double startingAngleR = startingAngle*(pi/180);
    double angleR= angle*(pi/180);
    std::string rulesCOPY=initiator;
    std::string result;
    for (int Iteration=0;Iteration!=nrIterations;Iteration++){
        result="";
        for(std::string::const_iterator it=rulesCOPY.begin();it!=rulesCOPY.end();it++){
            std::string itString;
            itString.assign(&(*it),&(*it)+1);
            bool stochastExist=false;
            std::vector<double> chances;
            std::vector<int> indexs;
            int index=0;
            if (itString == "+" || itString=="-" || itString=="(" || itString==")"){
                result+=(*it);
            }else{
                for (auto it2=stochasts.begin(); it2!=stochasts.end(); it2++){
                    std::string it2String;
                    it2String.assign(&(*it2).first,&(*it2).first+1);
                    if(itString == it2String){
                        double chance = std::stod((*it2).second);
                        chances.push_back(chance);
                        indexs.push_back(index);
                        stochastExist= true;
                    }
                    index++;
                }
                if (!stochastExist){
                    result+=LSystem1.get_replacement((*it));
                } else {
                    int count=0;
                    for(std::vector<double>::iterator it3=chances.begin();it3!=chances.end();it3++,count++){
                        int a = std::round((*it3)*100);
                        int sizeDouble=1;
                        for(auto it4=stochasts[count].second.begin();it4!=stochasts[count].second.end();it4++,sizeDouble++){
                            std::string it4String;
                            it4String.assign(&(*it4),&(*it4)+1);
                            if(it4String==" "){
                                break;
                            }
                        }
                        std::string temp = stochasts[count].second;
                        if (rand() % 100 <a){
                            result+=temp.erase(0,sizeDouble);
                            break;
                        } else if(it3==chances.end()-1){
                            result+=temp.erase(0,sizeDouble);
                            break;
                        }
                    }
                }
            }
        }
        rulesCOPY=result;
    }
    if (nrIterations==0){
        result=rulesCOPY;
    }
    double currentAngle=startingAngleR;
    std::pair<double,double> position = std::make_pair(0,0);
    Lines2D lines;
    std::vector<std::pair<std::pair<double,double>,double>> STACK;
    for(std::string::const_iterator it=result.begin();it!=result.end();it++){
        std::string itString;
        itString.assign(&(*it),&(*it)+1);
        if(itString=="-"){
            currentAngle-=angleR;
        } else if (itString=="+") {
            currentAngle += angleR;
        } else if(itString ==")") {
            position=(*(STACK.end()-1)).first;
            currentAngle=(*(STACK.end()-1)).second;
            STACK.pop_back();
        } else if(itString=="("){
            STACK.push_back(make_pair(position,currentAngle));
        } else{
            if (LSystem1.draw((*it))){
                std::pair<double,double> newposition = std::make_pair(position.first + cos(currentAngle),position.second + sin(currentAngle));
                Point2D point1;
                Point2D point2;
                point1.x=position.first;
                point1.y=position.second;
                point2.x=newposition.first;
                point2.y=newposition.second;
                Line2D line1;
                line1.p1=point1;
                line1.p2=point2;
                line1.color=img::Color(colorParsed[0]*255,colorParsed[1]*255,colorParsed[2]*255);
                lines.push_back(line1);
                position=newposition;
            } else {
                position=std::make_pair(position.first + cos(currentAngle),position.second + sin(currentAngle));
            }
        }
    }
    double xMin=0;
    double xMax=0;
    double yMin=0;
    double yMax=0;
    for(std::vector<Line2D>::iterator it=lines.begin();it!=lines.end();it++){
        vector<Point2D> p;
        p.push_back(it->p1);
        p.push_back(it->p2);
        for(int i=0; i!=2; i++){
            if (xMin>p[i].x){
                xMin=p[i].x;
            } else if(xMax<p[i].x){
                xMax=p[i].x;
            }
            if (yMin>p[i].y){
                yMin=p[i].y;
            } else if(yMax<p[i].y){
                yMax=p[i].y;
            }
        }
    }
    double xRange = std::abs(xMax) + std::abs(xMin);
    double yRange = std::abs(yMax) + std::abs(yMin);
    double xImage=sizeParsed*(xRange/ std::max(xRange,yRange));
    double yImage=sizeParsed*(yRange/ std::max(xRange,yRange));
    double d=0.95*(xImage/xRange);
    double dx = (xImage/2) - d*((xMin + xMax)/2);
    double dy = (yImage/2) - d*((yMin + yMax)/2);
    for(std::vector<Line2D>::iterator it=lines.begin();it!=lines.end();it++){
        (*it).p1.x = std::round((*it).p1.x*d+dx);
        (*it).p1.y = std::round((*it).p1.y*d+dy);
        (*it).p2.x = std::round((*it).p2.x*d+dx);
        (*it).p2.y = std::round((*it).p2.y*d+dy);
    }
    img::EasyImage image(round(xImage),round(yImage));
    img::Color backgroundcolorDone(backgroundcolorParsed[0]*255,backgroundcolorParsed[1]*255,backgroundcolorParsed[2]*255);
    backgroundcolorer(image,round(xImage),round(yImage),backgroundcolorDone);
    draw2DLines(lines,image);
    return image;
}

Matrix RotationX(const double angle){
    Matrix X;
    X(2,2) = cos(angle);
    X(2,3) = sin(angle);
    X(3,2) = -sin(angle);
    X(3,3) = cos(angle);
    return X;
}

Matrix RotationY(const double angle){
    Matrix Y;
    Y(1,1) = cos(angle);
    Y(1,3) = -sin(angle);
    Y(3,1) = sin(angle);
    Y(3,3) = cos(angle);
    return Y;
}

Matrix RotationZ(const double angle){
    Matrix Z;
    Z(1,1) = cos(angle);
    Z(1,2) = sin(angle);
    Z(2,1) = -sin(angle);
    Z(2,2) = cos(angle);
    return Z;
}

Matrix Scale(const double scale){
    Matrix S;
    S(1,1) = scale;
    S(2,2) = scale;
    S(3,3) = scale;
    return S;
}

Matrix Translation(const double a, const double b, const double c){
    Matrix T;
    T(4,1) = a;
    T(4,2) = b;
    T(4,3) = c;
    return T;
}

void transform(ThreeDfigures &Figure, const Matrix &m){
    for(vector<Point3D>::iterator it=Figure.points.begin(); it!=Figure.points.end(); it++){
        Vector3D Point = Vector3D::point(it->x,it->y,it->z);
        Point=Point*m;
        it->x=Point.x;
        it->y=Point.y;
        it->z=Point.z;
    }
}

Matrix eyePointTrans(const Vector3D &eyepoint){
    Matrix EYE;
    EYE(1,1) = -sin(eyepoint.y);
    EYE(1,2) = -(cos(eyepoint.y)*cos(eyepoint.z));
    EYE(1,3) = cos(eyepoint.y)*sin(eyepoint.z);
    EYE(2,1) = cos(eyepoint.y);
    EYE(2,2) = -(sin(eyepoint.y)*cos(eyepoint.z));
    EYE(2,3) = sin(eyepoint.y)*sin(eyepoint.z);
    EYE(3,2) = sin(eyepoint.z);
    EYE(3,3) = cos(eyepoint.z);
    EYE(4,3) = -eyepoint.x;
    return EYE;
}

void applyTransformations(Figures3D &figs,const Matrix &m){
    for(Figures3D::iterator it=figs.begin(); it!=figs.end(); it++){
        transform((*it),m);
    }
}

Lines2D doProjection(const Figures3D& figures){
    Lines2D lines;
    for(Figures3D::const_iterator it=figures.begin(); it!=figures.end(); it++){
        for(vector<Face3D> ::const_iterator it2=(*it).faces.begin(); it2!=(*it).faces.end(); it2++){
            vector<Point3D> facepoints;
            for(vector<int>::const_iterator it3=(*it2).point_indexes.begin(); it3!=(*it2).point_indexes.end(); it3++){
                Point3D point=it->points[(*it3)];
                facepoints.push_back(point);
            }
            Point3D firstpoint3D = (*facepoints.begin());
            Point2D backpoint2D;
            if(firstpoint3D.z == 0){
                backpoint2D.x=(-firstpoint3D.x);
                backpoint2D.y=(-firstpoint3D.y);
            } else {
                backpoint2D.x=(firstpoint3D.x/-firstpoint3D.z);
                backpoint2D.y=(firstpoint3D.y/-firstpoint3D.z);
            }
            Point2D copypoint2D=backpoint2D;
            Line2D line;
            Point2D frontpoint2D;
            for(vector<Point3D>::const_iterator it4=facepoints.begin()+1; it4!=facepoints.end(); it4++){
                Line2D emptyline;
                Point2D emptyfrontpoint2D;
                line =emptyline;
                frontpoint2D= emptyfrontpoint2D;
                if (it4->z ==0){
                    frontpoint2D.x=(-it4->x);
                    frontpoint2D.y=(-it4->y);
                } else{
                    frontpoint2D.x=(it4->x/-it4->z);
                    frontpoint2D.y=(it4->y/-it4->z);
                }
                line.p1=backpoint2D;
                line.p2=frontpoint2D;
                line.color=(*it).color;
                lines.push_back(line);
                backpoint2D = frontpoint2D;
            }
            line.p1=backpoint2D;
            line.p2=copypoint2D;
            line.color=(*it).color;
            lines.push_back(line);
        }
    }
    return lines;
}

ThreeDfigures Lsystem3D(const string &file){
    ThreeDfigures figure;
    std::ifstream IFP;
    IFP.open(file);
    LParser::LSystem3D LSystem3D = LParser::LSystem3D(IFP);
    IFP.close();
    const std::set<char> alphabet = LSystem3D.get_alphabet();
    const std::string initiator = LSystem3D.get_initiator();
    double angle = LSystem3D.get_angle();
    int nrIterations = LSystem3D.get_nr_iterations();
    std::vector<std::pair<char,std::string>> stochasts = LSystem3D.get_stochasts();
    double pi=3.14159265358979323846;
    double angleR= angle*(pi/180);
    std::string rulesCOPY=initiator;
    std::string result;
    for (int Iteration=0;Iteration!=nrIterations;Iteration++){
        result="";
        for(std::string::const_iterator it=rulesCOPY.begin();it!=rulesCOPY.end();it++){
            std::string itString;
            itString.assign(&(*it),&(*it)+1);
            bool stochastExist=false;
            int index=0;
            if (itString == "+" || itString=="-" || itString=="(" || itString==")" || itString=="&" || itString=="\\" || itString=="/" || itString=="^" || itString == "|"){
                result+=(*it);
            }else{
                result+=LSystem3D.get_replacement((*it));
            }
        }
        rulesCOPY=result;
    }
    if (nrIterations==0){
        result=rulesCOPY;
    }
    double currentAngle=angleR;
    Point3D position;
    Point3D OLDposition;
    position.setPoints(0,0,0);
    OLDposition.setPoints(0,0,0);
    Vector3D H = Vector3D::vector(1,0,0);
    Vector3D L = Vector3D::vector(0,1,0);
    Vector3D U = Vector3D::vector(0,0,1);
    Vector3D H_oud = Vector3D::vector(1,0,0);
    Vector3D L_oud = Vector3D::vector(0,1,0);
    Vector3D U_oud = Vector3D::vector(0,0,1);
    figure.points.push_back(position);
    vector<pair<Point3D,vector<Vector3D>>> STACK;
    for(std::string::const_iterator it=result.begin();it!=result.end();it++){
        std::string itString;
        itString.assign(&(*it),&(*it)+1);
        if(itString=="-"){
            H=(H_oud*cos(-currentAngle)) + (L_oud*sin(-currentAngle));
            L=((-H_oud)*sin(-currentAngle)) + (L_oud*cos(-currentAngle));
            H_oud=H;
            L_oud=L;
            U_oud=U;
        } else if (itString=="+") {
            H=(H_oud*cos(currentAngle)) + (L_oud*sin(currentAngle));
            L=((-H_oud)*sin(currentAngle)) + (L_oud*cos(currentAngle));
            H_oud=H;
            L_oud=L;
            U_oud=U;
        } else if(itString =="^") {
            H=(H_oud*cos(currentAngle)) + (U_oud*sin(currentAngle));
            U=((-H_oud)*sin(currentAngle)) + (U_oud*cos(currentAngle));
            H_oud=H;
            L_oud=L;
            U_oud=U;
        } else if(itString=="&"){
            H=(H_oud*cos(-currentAngle)) + (U_oud*sin(-currentAngle));
            U=((-H_oud)*sin(-currentAngle)) + (U_oud*cos(-currentAngle));
            H_oud=H;
            L_oud=L;
            U_oud=U;
        } else if(itString=="|") {
            H = -H_oud;
            L = -L_oud;
            H_oud=H;
            L_oud=L;
            U_oud=U;
        } else if(itString=="\\") {
            L=(L_oud*cos(currentAngle)) - (U_oud*sin(currentAngle));
            U=(L_oud*sin(currentAngle)) + (U_oud*cos(currentAngle));
            H_oud=H;
            L_oud=L;
            U_oud=U;
        }else if(itString=="/") {
            L=(L_oud*cos(-currentAngle)) - (U_oud*sin(-currentAngle));
            U=(L_oud*sin(-currentAngle)) + (U_oud*cos(-currentAngle));
            H_oud=H;
            L_oud=L;
            U_oud=U;
        } else if(itString=="(") {
            vector<Vector3D> vectors ={H,L,U};
            STACK.push_back(make_pair(OLDposition,vectors));
        }else if(itString==")") {
            OLDposition=(*(STACK.end()-1)).first;
            H=(*(STACK.end()-1)).second[0];
            L=(*(STACK.end()-1)).second[1];
            U=(*(STACK.end()-1)).second[2];
            H_oud=(*(STACK.end()-1)).second[0];
            L_oud=(*(STACK.end()-1)).second[1];
            U_oud=(*(STACK.end()-1)).second[2];
            STACK.pop_back();
        }else{
            if (LSystem3D.draw((*it))){
                position.z=OLDposition.z+H.z;
                position.y=OLDposition.y+H.y;
                position.x=OLDposition.x+H.x;
                figure.points.push_back(position);
                Face3D face;
                int count = 0;
                for(vector<Point3D>::iterator it2=figure.points.begin(); it2!=figure.points.end(); it2++,count++){
                    if (position.z==(*it2).z && position.y==(*it2).y && position.x==(*it2).x){
                        face.point_indexes.push_back(count);
                    } else if (OLDposition.z==(*it2).z && OLDposition.y==(*it2).y && OLDposition.x==(*it2).x){
                        face.point_indexes.push_back(count);
                    }
                }
                figure.faces.push_back(face);
                OLDposition.z=position.z;
                OLDposition.y=position.y;
                OLDposition.x=position.x;
            } else {
                OLDposition.z=OLDposition.z+H.z;
                OLDposition.y=OLDposition.y+H.y;
                OLDposition.x=OLDposition.x+H.x;
            }
        }
    }
    return figure;
}

void generatefractal(Figures3D& fractal,const int nr_iterations, const double scale){
    Matrix Ms = Scale(1/scale);
    for(int n=0; n!=nr_iterations; n++){
        Figures3D fractalREP;
        for(vector<ThreeDfigures>::const_iterator figure=fractal.begin(); figure!=fractal.end(); figure++){
            int count=0;
            for(vector<Point3D>::const_iterator point=figure->points.begin(); point!=figure->points.end(); point++, count++){
                ThreeDfigures copy=(*figure);
                transform(copy,Ms);
                Matrix T= Translation(figure->points[count].x-copy.points[count].x,figure->points[count].y-copy.points[count].y,figure->points[count].z-copy.points[count].z);
                transform(copy,T);
                fractalREP.push_back(copy);
            }
        }
        fractal=fractalREP;
    }
}

void generateMengerSponge(Figures3D& fractal,const int nr_iterations){
    Matrix Ms = Scale(1.0/3);
    for(int n=0; n!=nr_iterations; n++){
        Figures3D fractalREP;
        for(vector<ThreeDfigures>::const_iterator figure=fractal.begin(); figure!=fractal.end(); figure++){
            int count=0;
            for(vector<Point3D>::const_iterator point=figure->points.begin(); point!=figure->points.end(); point++, count++){
                if (count==0){
                    ThreeDfigures copy=(*figure);

                    ThreeDfigures copy2=(*figure);
                    ThreeDfigures copy3=(*figure);
                    ThreeDfigures copy4=(*figure);
                    ThreeDfigures copy5=(*figure);

                    ThreeDfigures copy6=(*figure);
                    ThreeDfigures copy7=(*figure);
                    ThreeDfigures copy8=(*figure);
                    ThreeDfigures copy9=(*figure);

                    ThreeDfigures copy10=(*figure);
                    ThreeDfigures copy11=(*figure);
                    ThreeDfigures copy12=(*figure);
                    ThreeDfigures copy13=(*figure);

                    transform(copy,Ms);
                    Matrix T= Translation(figure->points[count].x-copy.points[count].x,figure->points[count].y-copy.points[count].y,figure->points[count].z-copy.points[count].z);
                    transform(copy,T);
                    fractalREP.push_back(copy);

                    transform(copy2,Ms);
                    transform(copy3,Ms);
                    transform(copy4,Ms);
                    transform(copy5,Ms);
                    transform(copy6,Ms);
                    transform(copy7,Ms);
                    transform(copy8,Ms);
                    transform(copy9,Ms);
                    transform(copy10,Ms);
                    transform(copy11,Ms);
                    transform(copy12,Ms);
                    transform(copy13,Ms);

                    double xlength=abs(copy.points[0].x-copy.points[1].x);
                    double ylength=abs(copy.points[0].y-copy.points[1].y);
                    double zlength=abs(copy.points[0].z-copy.points[1].z);

                    Matrix T2= Translation(figure->points[count].x-xlength-copy2.points[count].x,figure->points[count].y-copy2.points[count].y,figure->points[count].z-copy2.points[count].z);
                    Matrix T3= Translation(figure->points[count].x-copy3.points[count].x,figure->points[count].y+ylength-copy3.points[count].y,figure->points[count].z-copy3.points[count].z);
                    Matrix T4= Translation(figure->points[count].x-xlength*2-copy5.points[count].x,figure->points[count].y+ylength-copy5.points[count].y,figure->points[count].z-copy5.points[count].z);
                    Matrix T5= Translation(figure->points[count].x-xlength-copy6.points[count].x,figure->points[count].y+ylength*2-copy6.points[count].y,figure->points[count].z-copy6.points[count].z);
                    Matrix T6= Translation(figure->points[count].x-copy4.points[count].x,figure->points[count].y-copy4.points[count].y,figure->points[count].z+xlength-copy4.points[count].z);
                    Matrix T7= Translation(figure->points[count].x-xlength*2-copy7.points[count].x,figure->points[count].y-copy7.points[count].y,figure->points[count].z+xlength-copy7.points[count].z);
                    Matrix T8= Translation(figure->points[count].x-copy8.points[count].x,figure->points[count].y+ylength*2-copy8.points[count].y,figure->points[count].z+xlength-copy8.points[count].z);
                    Matrix T9= Translation(figure->points[count].x-xlength*2-copy9.points[count].x,figure->points[count].y+ylength*2-copy9.points[count].y,figure->points[count].z+xlength-copy9.points[count].z);
                    Matrix T10= Translation(figure->points[count].x-xlength-copy10.points[count].x,figure->points[count].y-copy10.points[count].y,figure->points[count].z+xlength*2-copy10.points[count].z);
                    Matrix T11= Translation(figure->points[count].x-copy11.points[count].x,figure->points[count].y+ylength-copy11.points[count].y,figure->points[count].z+xlength*2-copy11.points[count].z);
                    Matrix T12= Translation(figure->points[count].x-xlength*2-copy12.points[count].x,figure->points[count].y+ylength-copy12.points[count].y,figure->points[count].z+xlength*2-copy12.points[count].z);
                    Matrix T13= Translation(figure->points[count].x-xlength-copy13.points[count].x,figure->points[count].y+ylength*2-copy13.points[count].y,figure->points[count].z+xlength*2-copy13.points[count].z);

                    transform(copy2,T2);
                    transform(copy3,T3);
                    transform(copy4,T4);
                    transform(copy5,T5);
                    transform(copy6,T6);
                    transform(copy7,T7);
                    transform(copy8,T8);
                    transform(copy9,T9);
                    transform(copy10,T10);
                    transform(copy11,T11);
                    transform(copy12,T12);
                    transform(copy13,T13);

                    fractalREP.push_back(copy2);
                    fractalREP.push_back(copy3);
                    fractalREP.push_back(copy4);
                    fractalREP.push_back(copy5);
                    fractalREP.push_back(copy6);
                    fractalREP.push_back(copy7);
                    fractalREP.push_back(copy8);
                    fractalREP.push_back(copy9);
                    fractalREP.push_back(copy10);
                    fractalREP.push_back(copy11);
                    fractalREP.push_back(copy12);
                    fractalREP.push_back(copy13);

                } else {
                    ThreeDfigures copy=(*figure);
                    transform(copy,Ms);
                    Matrix T= Translation(figure->points[count].x-copy.points[count].x,figure->points[count].y-copy.points[count].y,figure->points[count].z-copy.points[count].z);
                    transform(copy,T);
                    fractalREP.push_back(copy);
                }
            }
        }
        fractal=fractalREP;
    }
}

void generateThickFigure(Figures3D &figures,const double r, const int n, const int m, bool color){
    ThreeDfigures temp = (*figures.begin());
    figures.clear();
    Matrix S=Scale(r);
    Lichamen3D body;
    for(vector<Point3D>::const_iterator point=temp.points.begin(); point!=temp.points.end(); point++){
        ThreeDfigures sphere=body.createSphere(m);
        sphere.color=temp.color;
        sphere.ambientReflection=temp.ambientReflection;
        sphere.reflectionCoefficient=temp.reflectionCoefficient;
        sphere.specularReflection=temp.specularReflection;
        sphere.diffuseReflection=temp.diffuseReflection;
        transform(sphere, S);
        transform(sphere, Translation(point->x,point->y,point->z));
        figures.push_back(sphere);
    }
    for(vector<Face3D> ::const_iterator face=temp.faces.begin(); face!=temp.faces.end(); face++){
        vector<Point3D> facepoints;
        for(vector<int>::const_iterator index=face->point_indexes.begin(); index!=face->point_indexes.end(); index++){
            Point3D point=temp.points[*index];
            facepoints.push_back(point);
        }
        Point3D firstpoint3D = (*facepoints.begin());
        Point3D firstpoint3Dcopy = (*facepoints.begin());
        for(vector<Point3D>::const_iterator point=facepoints.begin()+1; point!=facepoints.end(); point++){
            Vector3D p1 = Vector3D::point(firstpoint3D.x,firstpoint3D.y,firstpoint3D.z);
            Vector3D p2 = Vector3D::point(point->x,point->y,point->z);
            Vector3D tempV = p2-p1;
            double h=tempV.length()/r;
            ThreeDfigures cylinder;
            if(color){
                cylinder = body.createCylinderZ(n,h);
            } else {
                if(h<1){
                    continue;
                }
                cylinder = body.createCylinder(n,h);
            }
            cylinder.color=temp.color;
            cylinder.ambientReflection=temp.ambientReflection;
            cylinder.reflectionCoefficient=temp.reflectionCoefficient;
            cylinder.specularReflection=temp.specularReflection;
            cylinder.diffuseReflection=temp.diffuseReflection;
            double r2 = tempV.length();
            double theta = atan2(tempV.y,tempV.x);
            double phi = acos((tempV.z/r2));
            Matrix Rotation1Z = RotationY(phi);
            Matrix Rotation1X = RotationZ(theta);
            transform(cylinder,S);
            transform(cylinder,Rotation1Z);
            transform(cylinder,Rotation1X);
            transform(cylinder, Translation(p1.x,p1.y,p1.z));
            firstpoint3D = *point;
            figures.push_back(cylinder);
        }
        Vector3D p1 = Vector3D::point(firstpoint3Dcopy.x,firstpoint3Dcopy.y,firstpoint3Dcopy.z);
        Vector3D p2 = Vector3D::point(firstpoint3D.x,firstpoint3D.y,firstpoint3D.z);
        Vector3D tempV = p2-p1;
        double h=tempV.length()/r;
        ThreeDfigures cylinder;
        if(color){
            cylinder = body.createCylinderZ(n,h);
        } else {
            cylinder = body.createCylinder(n,h);
        }
        cylinder.color=temp.color;
        cylinder.ambientReflection=temp.ambientReflection;
        cylinder.reflectionCoefficient=temp.reflectionCoefficient;
        cylinder.specularReflection=temp.specularReflection;
        cylinder.diffuseReflection=temp.diffuseReflection;
        double r2 = tempV.length();
        double theta = atan2(tempV.y,tempV.x);
        double phi = acos((tempV.z/r2));
        Matrix Rotation1Z = RotationY(phi);
        Matrix Rotation1X = RotationZ(theta);
        transform(cylinder,S);
        transform(cylinder,Rotation1Z);
        transform(cylinder,Rotation1X);
        transform(cylinder, Translation(p1.x,p1.y,p1.z));
        figures.push_back(cylinder);
    }
}

img::EasyImage ThreeDsystem(const ini::Configuration &configuration){
    ini::Section general = configuration.operator[]("General");
    double pi=3.14159265358979323846;
    ini::Entry size = general.operator[]("size");
    ini::Entry backgroundcolor = general.operator[]("backgroundcolor");
    ini::Entry nrFigures = general.operator[]("nrFigures");
    ini::Entry eye = general.operator[]("eye");

    int sizeParsed;
    ini::DoubleTuple backgroundcolorParsed;
    int nrFiguresParsed;
    ini::DoubleTuple eyeParsed;
    double fractalScaleParsed;
    int nrIterationsParsed;
    bool fractal = false;
    bool MengerSponge = false;
    bool ThickLine=false;

    size.as_int_if_exists(sizeParsed);
    backgroundcolor.as_double_tuple_if_exists(backgroundcolorParsed);
    nrFigures.as_int_if_exists(nrFiguresParsed);
    eye.as_double_tuple_if_exists(eyeParsed);

    vector<ThreeDfigures> figures;
    
    for(int count=0; count!=nrFiguresParsed; count++){
        ThreeDfigures figureParsed;
        ini::Section Figure = configuration.operator[]("Figure" + to_string(count));
        ini::Entry type = Figure.operator[]("type");
        ini::Entry rotateX = Figure.operator[]("rotateX");
        ini::Entry rotateY = Figure.operator[]("rotateY");
        ini::Entry rotateZ = Figure.operator[]("rotateZ");
        ini::Entry scale = Figure.operator[]("scale");
        ini::Entry center = Figure.operator[]("center");
        ini::Entry color = Figure.operator[]("color");

        std::string typeParsed;
        double rotateXParsed;
        double rotateYParsed;
        double rotateZParsed;
        double scaleParsed;
        ini::DoubleTuple centerParsed;
        ini::DoubleTuple colorParsed;
        int nrPointsParsed;
        int nrLinesParsed;

        type.as_string_if_exists(typeParsed);
        color.as_double_tuple_if_exists(colorParsed);
        rotateX.as_double_if_exists(rotateXParsed);
        rotateY.as_double_if_exists(rotateYParsed);
        rotateZ.as_double_if_exists(rotateZParsed);
        scale.as_double_if_exists(scaleParsed);
        center.as_double_tuple_if_exists(centerParsed);
        if(typeParsed=="ThickLineDrawing" || typeParsed=="ThickCube" || typeParsed=="ThickOctahedron" || typeParsed=="ThickDodecahedron" || typeParsed=="ThickIcosahedron" || typeParsed=="ThickTetrahedron" || typeParsed=="Thick3DLSystem" || typeParsed=="ThickBuckyBall"){
            ThickLine=true;
            typeParsed=typeParsed.substr(5,string::npos);
        }
        Lichamen3D body;
        if (typeParsed=="Cube"){
            figureParsed = body.createCube();
        } else if (typeParsed=="Tetrahedron"){
            figureParsed = body.createTetrahedron();
        } else if (typeParsed=="Octahedron"){
            figureParsed = body.createOctahedron();
        } else if (typeParsed=="Icosahedron"){
            figureParsed = body.createIcosahedron();
        } else if (typeParsed=="Dodecahedron"){
            figureParsed = body.createDodecahedron();
        } else if (typeParsed=="Cylinder"){
            ini::Entry n = Figure.operator[]("n");
            ini::Entry h = Figure.operator[]("height");
            int nParsed;
            double hParsed;
            n.as_int_if_exists(nParsed);
            h.as_double_if_exists(hParsed);
            figureParsed = body.createCylinder(n,hParsed);
        } else if (typeParsed=="Cone"){
            ini::Entry n = Figure.operator[]("n");
            ini::Entry h = Figure.operator[]("height");
            int nParsed;
            double hParsed;
            n.as_int_if_exists(nParsed);
            h.as_double_if_exists(hParsed);
            figureParsed = body.createCone(n,hParsed);
        } else if (typeParsed=="Sphere"){
            ini::Entry n = Figure.operator[]("n");
            int nParsed;
            n.as_int_if_exists(nParsed);
            figureParsed = body.createSphere(nParsed);
        } else if (typeParsed=="Torus"){
            ini::Entry r = Figure.operator[]("r");
            double rParsed;
            r.as_double_if_exists(rParsed);
            ini::Entry R = Figure.operator[]("R");
            double RParsed;
            R.as_double_if_exists(RParsed);
            ini::Entry m = Figure.operator[]("m");
            int mParsed;
            m.as_int_if_exists(mParsed);
            ini::Entry n = Figure.operator[]("n");
            int nParsed;
            n.as_int_if_exists(nParsed);
            figureParsed = body.createTorus(rParsed, RParsed, mParsed, nParsed);
        } else if (typeParsed=="3DLSystem"){
            ini::Entry inputfile3D = Figure.operator[]("inputfile");
            string inputfile3DParsed;
            inputfile3D.as_string_if_exists(inputfile3DParsed);
            figureParsed = Lsystem3D(inputfile3DParsed);
        } else if (typeParsed=="FractalCube"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createCube();
        } else if (typeParsed=="BuckyBall"){
            figureParsed = body.createBuckyBall();
        } else if (typeParsed=="FractalBuckyBall"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createBuckyBall();
        } else if (typeParsed=="FractalDodecahedron"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createDodecahedron();
        }else if (typeParsed=="FractalIcosahedron"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createIcosahedron();
        } else if (typeParsed=="FractalOctahedron"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createOctahedron();
        } else if (typeParsed=="FractalTetrahedron"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createTetrahedron();
        } else if (typeParsed=="MengerSponge"){
            MengerSponge = true;
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createCube();
        } else {
            ini::Entry nrPoints = Figure.operator[]("nrPoints");
            ini::Entry nrLines = Figure.operator[]("nrLines");

            nrPoints.as_int_if_exists(nrPointsParsed);
            nrLines.as_int_if_exists(nrLinesParsed);

            for (int count2 = 0; count2 != nrPointsParsed; count2++) {
                ini::Entry point = Figure.operator[]("point" + to_string(count2));
                ini::DoubleTuple pointParsed;
                point.as_double_tuple_if_exists(pointParsed);
                Point3D point_3D;
                point_3D.x = pointParsed[0];
                point_3D.y = pointParsed[1];
                point_3D.z = pointParsed[2];
                figureParsed.points.push_back(point_3D);
            }

            for (int count3 = 0; count3 != nrLinesParsed; count3++) {
                ini::Entry line = Figure.operator[]("line" + to_string(count3));
                ini::DoubleTuple lineParsed;
                line.as_double_tuple_if_exists(lineParsed);
                Face3D face;
                for (ini::DoubleTuple::iterator it = lineParsed.begin(); it != lineParsed.end(); it++) {
                    face.point_indexes.push_back((*it));
                }
                figureParsed.faces.push_back(face);
            }
        }
        figureParsed.color=img::Color(colorParsed[0]*255,colorParsed[1]*255,colorParsed[2]*255);
        transform(figureParsed, Scale(scaleParsed));
        if (rotateXParsed != 0){
            transform(figureParsed, RotationX(rotateXParsed*pi/180));
        }
        if (rotateYParsed != 0){
            transform(figureParsed, RotationY(rotateYParsed*pi/180));
        }
        if (rotateZParsed != 0){
            transform(figureParsed, RotationZ(rotateZParsed*pi/180));
        }
        transform(figureParsed, Translation(centerParsed[0],centerParsed[1],centerParsed[2]));
        Figures3D tempfigures;
        if (fractal){
            tempfigures.push_back(figureParsed);
            generatefractal(tempfigures,nrIterationsParsed,fractalScaleParsed);
            figures.insert(figures.end(),tempfigures.begin(),tempfigures.end());
        } else if(MengerSponge){
            tempfigures.push_back(figureParsed);
            generateMengerSponge(tempfigures,nrIterationsParsed);
            figures.insert(figures.end(),tempfigures.begin(),tempfigures.end());
        } else if (ThickLine){
            ini::Entry r = Figure.operator[]("radius");
            double rParsed;
            r.as_double_if_exists(rParsed);
            ini::Entry m = Figure.operator[]("m");
            int mParsed;
            m.as_int_if_exists(mParsed);
            ini::Entry n = Figure.operator[]("n");
            int nParsed;
            n.as_int_if_exists(nParsed);
            tempfigures.push_back(figureParsed);
            generateThickFigure(tempfigures,rParsed,nParsed,mParsed, 0);
            figures.insert(figures.end(),tempfigures.begin(),tempfigures.end());
        } else {
            figures.push_back(figureParsed);
        }
    }
    double r = sqrt((pow(eyeParsed[0],2)+pow(eyeParsed[1],2)+pow(eyeParsed[2],2)));
    double theta=atan2(eyeParsed[1],eyeParsed[0]);
    double phi=acos((eyeParsed[2]/r));
    Matrix V = eyePointTrans(Vector3D::point(r,theta,phi));
    applyTransformations(figures,V);
    Lines2D lines = doProjection(figures);
    double xMax=0;
    double xMin=0;
    double yMax=0;
    double yMin=0;
    for(std::vector<Line2D>::iterator it=lines.begin();it!=lines.end();it++){
        vector<Point2D> p;
        p.push_back(it->p1);
        p.push_back(it->p2);
        for(int i=0; i!=2; i++){
            if (xMin>p[i].x){
                xMin=p[i].x;
            } else if(xMax<p[i].x){
                xMax=p[i].x;
            }
            if (yMin>p[i].y){
                yMin=p[i].y;
            } else if(yMax<p[i].y){
                yMax=p[i].y;
            }
        }
    }
    double xRange = std::abs(xMax) + std::abs(xMin);
    double yRange = std::abs(yMax) + std::abs(yMin);
    double xImage=sizeParsed*(xRange/ std::max(xRange,yRange));
    double yImage=sizeParsed*(yRange/ std::max(xRange,yRange));
    double d=0.95*(xImage/xRange);
    double dx = (xImage/2) - d*((xMin + xMax)/2);
    double dy = (yImage/2) - d*((yMin + yMax)/2);
    for(std::vector<Line2D>::iterator it=lines.begin();it!=lines.end();it++){
        (*it).p1.x = std::round((*it).p1.x*d+dx);
        (*it).p1.y = std::round((*it).p1.y*d+dy);
        (*it).p2.x = std::round((*it).p2.x*d+dx);
        (*it).p2.y = std::round((*it).p2.y*d+dy);
    }
    img::EasyImage image(round(xImage),round(yImage));
    img::Color backgroundcolorDone(backgroundcolorParsed[0]*255,backgroundcolorParsed[1]*255,backgroundcolorParsed[2]*255);
    backgroundcolorer(image,round(xImage),round(yImage),backgroundcolorDone);
    draw2DLines(lines, image);
    return image;
}

Lines2D doProjectionZBuffering(const Figures3D& figures){
    Lines2D lines;
    for(Figures3D::const_iterator it=figures.begin(); it!=figures.end(); it++){
        for(vector<Face3D> ::const_iterator it2=(*it).faces.begin(); it2!=(*it).faces.end(); it2++){
            vector<Point3D> facepoints;
            for(vector<int>::const_iterator it3=(*it2).point_indexes.begin(); it3!=(*it2).point_indexes.end(); it3++){
                Point3D point=it->points[(*it3)];
                facepoints.push_back(point);
            }
            Point3D firstpoint3D = (*facepoints.begin());
            Point2D backpoint2D;
            Point3D backpoint3D=firstpoint3D;
            if(firstpoint3D.z == 0){
                backpoint2D.x=(-firstpoint3D.x);
                backpoint2D.y=(-firstpoint3D.y);
            } else {
                backpoint2D.x=(firstpoint3D.x/-firstpoint3D.z);
                backpoint2D.y=(firstpoint3D.y/-firstpoint3D.z);
            }
            Point2D copypoint2D=backpoint2D;
            Point3D copypoint3D=backpoint3D;
            Line2D line;
            double copyZ=firstpoint3D.z;
            double PrevZ=firstpoint3D.z;
            Point2D frontpoint2D;
            for(vector<Point3D>::const_iterator it4=facepoints.begin()+1; it4!=facepoints.end(); it4++){
                Line2D emptyline;
                Point2D emptyfrontpoint2D;
                line =emptyline;
                frontpoint2D= emptyfrontpoint2D;
                if (it4->z ==0){
                    frontpoint2D.x=(-it4->x);
                    frontpoint2D.y=(-it4->y);
                } else{
                    frontpoint2D.x=(it4->x/-it4->z);
                    frontpoint2D.y=(it4->y/-it4->z);
                }
                line.p1=backpoint2D;
                line.p2=frontpoint2D;
                line.color=(*it).color;
                line.z1=1/PrevZ;
                line.z2=1/it4->z;
                line.Op1=backpoint3D;
                line.Op2=(*it4);
                lines.push_back(line);
                backpoint2D = frontpoint2D;
                backpoint3D=(*it4);
                PrevZ=it4->z;
            }
            line.p1=backpoint2D;
            line.p2=copypoint2D;
            line.z1=1/PrevZ;
            line.z2=1/copyZ;
            line.Op1=backpoint3D;
            line.Op2=copypoint3D;
            line.color=(*it).color;
            lines.push_back(line);
        }
    }
    return lines;
}

img::EasyImage ThreeDsystemZBuffered(const ini::Configuration &configuration){
    ini::Section general = configuration.operator[]("General");
    double pi=3.14159265358979323846;
    ini::Entry size = general.operator[]("size");
    ini::Entry backgroundcolor = general.operator[]("backgroundcolor");
    ini::Entry nrFigures = general.operator[]("nrFigures");
    ini::Entry eye = general.operator[]("eye");

    int sizeParsed;
    ini::DoubleTuple backgroundcolorParsed;
    int nrFiguresParsed;
    ini::DoubleTuple eyeParsed;
    double fractalScaleParsed;
    int nrIterationsParsed;
    bool fractal = false;
    bool MengerSponge = false;
    bool ThickLine=false;


    size.as_int_if_exists(sizeParsed);
    backgroundcolor.as_double_tuple_if_exists(backgroundcolorParsed);
    nrFigures.as_int_if_exists(nrFiguresParsed);
    eye.as_double_tuple_if_exists(eyeParsed);

    vector<ThreeDfigures> figures;

    for(int count=0; count!=nrFiguresParsed; count++){
        ThreeDfigures figureParsed;
        ini::Section Figure = configuration.operator[]("Figure" + to_string(count));
        ini::Entry type = Figure.operator[]("type");
        ini::Entry rotateX = Figure.operator[]("rotateX");
        ini::Entry rotateY = Figure.operator[]("rotateY");
        ini::Entry rotateZ = Figure.operator[]("rotateZ");
        ini::Entry scale = Figure.operator[]("scale");
        ini::Entry center = Figure.operator[]("center");
        ini::Entry color = Figure.operator[]("color");

        std::string typeParsed;
        double rotateXParsed;
        double rotateYParsed;
        double rotateZParsed;
        double scaleParsed;
        ini::DoubleTuple centerParsed;
        ini::DoubleTuple colorParsed;
        int nrPointsParsed;
        int nrLinesParsed;

        type.as_string_if_exists(typeParsed);
        color.as_double_tuple_if_exists(colorParsed);
        rotateX.as_double_if_exists(rotateXParsed);
        rotateY.as_double_if_exists(rotateYParsed);
        rotateZ.as_double_if_exists(rotateZParsed);
        scale.as_double_if_exists(scaleParsed);
        center.as_double_tuple_if_exists(centerParsed);
        if(typeParsed=="ThickLineDrawing" || typeParsed=="ThickCube" || typeParsed=="ThickOctahedron" || typeParsed=="ThickDodecahedron" || typeParsed=="ThickIcosahedron" || typeParsed=="ThickTetrahedron" || typeParsed=="Thick3DLSystem" || typeParsed=="ThickBuckyBall"){
            ThickLine=true;
            typeParsed=typeParsed.substr(5,string::npos);
        }
        Lichamen3D body;
        if (typeParsed=="Cube"){
            figureParsed = body.createCube();
        } else if (typeParsed=="Tetrahedron"){
            figureParsed = body.createTetrahedron();
        } else if (typeParsed=="Octahedron"){
            figureParsed = body.createOctahedron();
        } else if (typeParsed=="Icosahedron"){
            figureParsed = body.createIcosahedron();
        } else if (typeParsed=="Dodecahedron"){
            figureParsed = body.createDodecahedron();
        } else if (typeParsed=="Cylinder"){
            ini::Entry n = Figure.operator[]("n");
            ini::Entry h = Figure.operator[]("height");
            int nParsed;
            double hParsed;
            n.as_int_if_exists(nParsed);
            h.as_double_if_exists(hParsed);
            figureParsed = body.createCylinder(n,hParsed);
        } else if (typeParsed=="Cone"){
            ini::Entry n = Figure.operator[]("n");
            ini::Entry h = Figure.operator[]("height");
            int nParsed;
            double hParsed;
            n.as_int_if_exists(nParsed);
            h.as_double_if_exists(hParsed);
            figureParsed = body.createCone(n,hParsed);
        } else if (typeParsed=="Sphere"){
            ini::Entry n = Figure.operator[]("n");
            int nParsed;
            n.as_int_if_exists(nParsed);
            figureParsed = body.createSphere(nParsed);
        } else if (typeParsed=="Torus"){
            ini::Entry r = Figure.operator[]("r");
            double rParsed;
            r.as_double_if_exists(rParsed);
            ini::Entry R = Figure.operator[]("R");
            double RParsed;
            R.as_double_if_exists(RParsed);
            ini::Entry m = Figure.operator[]("m");
            int mParsed;
            m.as_int_if_exists(mParsed);
            ini::Entry n = Figure.operator[]("n");
            int nParsed;
            n.as_int_if_exists(nParsed);
            figureParsed = body.createTorus(rParsed, RParsed, mParsed, nParsed);
        } else if (typeParsed=="3DLSystem"){
            ini::Entry inputfile3D = Figure.operator[]("inputfile");
            string inputfile3DParsed;
            inputfile3D.as_string_if_exists(inputfile3DParsed);
            figureParsed = Lsystem3D(inputfile3DParsed);
        } else if (typeParsed=="FractalCube"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createCube();
        } else if (typeParsed=="BuckyBall"){
            figureParsed = body.createBuckyBall();
        } else if (typeParsed=="FractalBuckyBall"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createBuckyBall();
        } else if (typeParsed=="FractalDodecahedron"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createDodecahedron();
        }else if (typeParsed=="FractalIcosahedron"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createIcosahedron();
        } else if (typeParsed=="FractalOctahedron"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createOctahedron();
        } else if (typeParsed=="FractalTetrahedron"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createTetrahedron();
        } else if (typeParsed=="MengerSponge"){
            MengerSponge = true;
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createCube();
        } else {
            ini::Entry nrPoints = Figure.operator[]("nrPoints");
            ini::Entry nrLines = Figure.operator[]("nrLines");

            nrPoints.as_int_if_exists(nrPointsParsed);
            nrLines.as_int_if_exists(nrLinesParsed);

            for (int count2 = 0; count2 != nrPointsParsed; count2++) {
                ini::Entry point = Figure.operator[]("point" + to_string(count2));
                ini::DoubleTuple pointParsed;
                point.as_double_tuple_if_exists(pointParsed);
                Point3D point_3D;
                point_3D.x = pointParsed[0];
                point_3D.y = pointParsed[1];
                point_3D.z = pointParsed[2];
                figureParsed.points.push_back(point_3D);
            }

            for (int count3 = 0; count3 != nrLinesParsed; count3++) {
                ini::Entry line = Figure.operator[]("line" + to_string(count3));
                ini::DoubleTuple lineParsed;
                line.as_double_tuple_if_exists(lineParsed);
                Face3D face;
                for (ini::DoubleTuple::iterator it = lineParsed.begin(); it != lineParsed.end(); it++) {
                    face.point_indexes.push_back((*it));
                }
                figureParsed.faces.push_back(face);
            }
        }
        figureParsed.color=img::Color(colorParsed[0]*255,colorParsed[1]*255,colorParsed[2]*255);
        transform(figureParsed, Scale(scaleParsed));
        if (rotateXParsed != 0){
            transform(figureParsed, RotationX(rotateXParsed*pi/180));
        }
        if (rotateYParsed != 0){
            transform(figureParsed, RotationY(rotateYParsed*pi/180));
        }
        if (rotateZParsed != 0){
            transform(figureParsed, RotationZ(rotateZParsed*pi/180));
        }
        transform(figureParsed, Translation(centerParsed[0],centerParsed[1],centerParsed[2]));
        Figures3D tempfigures;
        if (fractal){
            tempfigures.push_back(figureParsed);
            generatefractal(tempfigures,nrIterationsParsed,fractalScaleParsed);
            figures.insert(figures.end(),tempfigures.begin(),tempfigures.end());
        } else if(MengerSponge){
            tempfigures.push_back(figureParsed);
            generateMengerSponge(tempfigures,nrIterationsParsed);
            figures.insert(figures.end(),tempfigures.begin(),tempfigures.end());
        } else if (ThickLine){
            ini::Entry r = Figure.operator[]("radius");
            double rParsed;
            r.as_double_if_exists(rParsed);
            ini::Entry m = Figure.operator[]("m");
            int mParsed;
            m.as_int_if_exists(mParsed);
            ini::Entry n = Figure.operator[]("n");
            int nParsed;
            n.as_int_if_exists(nParsed);
            tempfigures.push_back(figureParsed);
            generateThickFigure(tempfigures,rParsed,nParsed,mParsed, 0);
            figures.insert(figures.end(),tempfigures.begin(),tempfigures.end());
        } else {
            figures.push_back(figureParsed);
        }
    }
    double r = sqrt((pow(eyeParsed[0],2)+pow(eyeParsed[1],2)+pow(eyeParsed[2],2)));
    double theta=atan2(eyeParsed[1],eyeParsed[0]);
    double phi=acos((eyeParsed[2]/r));
    Matrix V = eyePointTrans(Vector3D::point(r,theta,phi));
    applyTransformations(figures,V);
    Lines2D lines = doProjectionZBuffering(figures);
    double xMax=0;
    double xMin=0;
    double yMax=0;
    double yMin=0;
    for(std::vector<Line2D>::iterator it=lines.begin();it!=lines.end();it++){
        vector<Point2D> p;
        p.push_back(it->p1);
        p.push_back(it->p2);
        for(int i=0; i!=2; i++){
            if (xMin>p[i].x){
                xMin=p[i].x;
            } else if(xMax<p[i].x){
                xMax=p[i].x;
            }
            if (yMin>p[i].y){
                yMin=p[i].y;
            } else if(yMax<p[i].y){
                yMax=p[i].y;
            }
        }
    }
    double xRange = std::abs(xMax) + std::abs(xMin);
    double yRange = std::abs(yMax) + std::abs(yMin);
    double xImage=sizeParsed*(xRange/ std::max(xRange,yRange));
    double yImage=sizeParsed*(yRange/ std::max(xRange,yRange));
    double d=0.95*(xImage/xRange);
    double dx = (xImage/2) - d*((xMin + xMax)/2);
    double dy = (yImage/2) - d*((yMin + yMax)/2);
    for(std::vector<Line2D>::iterator it=lines.begin();it!=lines.end();it++){
        (*it).p1.x = std::round(d*(*it).p1.x+dx);
        (*it).p1.y = std::round(d*(*it).p1.y+dy);
        (*it).p2.x = std::round(d*(*it).p2.x+dx);
        (*it).p2.y = std::round(d*(*it).p2.y+dy);
    }
    img::EasyImage image(std::round(xImage),std::round(yImage));
    img::Color backgroundcolorDone(backgroundcolorParsed[0]*255,backgroundcolorParsed[1]*255,backgroundcolorParsed[2]*255);
    ZBuffer Z(std::round(xImage),std::round(yImage),backgroundcolorDone);
    for(std::vector<Line2D>::const_iterator it=lines.begin();it!=lines.end();it++){
        int a= round(sqrt(pow((*it).p1.x-(*it).p2.x,2)+pow((*it).p1.y-(*it).p2.y,2)));
        if(a<=0){
            continue;
        }
        for(int i=a; i!=-1; i--){
            double div=(double)i/a;
            int xi = round(div*(*it).p1.x+(1-div)*(*it).p2.x);
            int yi = round(div*(*it).p1.y+(1-div)*(*it).p2.y);
            double z = div*(*it).z1 + (1-div)*(*it).z2;
            if(Z.buffer[xi][yi].first>z){
                Z.buffer[xi][yi].first=z;
                Z.buffer[xi][yi].second=(*it).color;
            }
        }
    }
    int width=0;
    int height=0;
    for(vector<vector<pair<double,img::Color>>>::const_iterator it=Z.buffer.begin(); it!=Z.buffer.end(); it++,width++){
        height=0;
        for(vector<pair<double,img::Color>>::const_iterator it2=(*it).begin(); it2!=(*it).end(); it2++,height++){
            image(width,height)=(it2->second);
        }
    }
    return image;
}

vector<Face3D> triangulate(const vector<Face3D>& faces){
    vector<Face3D> mainfaces;
    Face3D temp;
    for(vector<Face3D> ::const_iterator it=faces.begin(); it!=faces.end(); it++){
        if ((*it).point_indexes.size()<=2){
            for(vector<Face3D> ::const_iterator it2=faces.begin(); it2!=faces.end(); it2++){
                for(vector<int>::const_iterator it3=(*it2).point_indexes.begin(); it3!=(*it2).point_indexes.end(); it3++){
                    temp.point_indexes.push_back((*it3));
                }
            }
            mainfaces.push_back(temp);
            mainfaces = triangulate(mainfaces);
            break;
        }
        for(vector<int>::const_iterator it2=(*it).point_indexes.begin()+1; it2!=(*it).point_indexes.end()-1; it2++){
            temp.point_indexes={(*it).point_indexes[0],(*it2),(*(it2+1))};
            mainfaces.push_back(temp);
        }
    }
    return mainfaces;
}

Lines2D doProjectionZBufferingcolor(const Figures3D& figures){
    Lines2D lines;
    for(Figures3D::const_iterator it=figures.begin(); it!=figures.end(); it++){
        vector<Face3D> mainfaces;
        mainfaces=triangulate(it->faces);
        for(vector<Face3D> ::const_iterator it2=mainfaces.begin(); it2!=mainfaces.end(); it2++){
            vector<Point3D> facepoints;
            for(vector<int>::const_iterator it3=(*it2).point_indexes.begin(); it3!=(*it2).point_indexes.end(); it3++){
                Point3D point=it->points[(*it3)];
                facepoints.push_back(point);
            }
            Point3D firstpoint3D = (*facepoints.begin());
            Point2D backpoint2D;
            Point3D backpoint3D=firstpoint3D;
            if(firstpoint3D.z == 0){
                backpoint2D.x=(-firstpoint3D.x);
                backpoint2D.y=(-firstpoint3D.y);
            } else {
                backpoint2D.x=(firstpoint3D.x/-firstpoint3D.z);
                backpoint2D.y=(firstpoint3D.y/-firstpoint3D.z);
            }
            Point2D copypoint2D=backpoint2D;
            Point3D copypoint3D=backpoint3D;
            Line2D line;
            double copyZ=firstpoint3D.z;
            double PrevZ=firstpoint3D.z;
            Point2D frontpoint2D;
            for(vector<Point3D>::const_iterator it4=facepoints.begin()+1; it4!=facepoints.end(); it4++){
                Line2D emptyline;
                Point2D emptyfrontpoint2D;
                line =emptyline;
                frontpoint2D= emptyfrontpoint2D;
                if (it4->z ==0){
                    frontpoint2D.x=(-it4->x);
                    frontpoint2D.y=(-it4->y);
                } else{
                    frontpoint2D.x=(it4->x/-it4->z);
                    frontpoint2D.y=(it4->y/-it4->z);
                }
                line.p1=backpoint2D;
                line.p2=frontpoint2D;
                line.color=(*it).color;
                line.diffuse=(*it).diffuseReflection;
                line.reflectionCoefficient=(*it).reflectionCoefficient;
                line.specular=(*it).specularReflection;
                line.z1=1/PrevZ;
                line.z2=1/it4->z;
                line.Op1=backpoint3D;
                line.Op2=(*it4);
                lines.push_back(line);
                backpoint2D = frontpoint2D;
                backpoint3D=(*it4);
                PrevZ=it4->z;
            }
            line.p1=backpoint2D;
            line.p2=copypoint2D;
            line.z1=1/PrevZ;
            line.z2=1/copyZ;
            line.Op1=backpoint3D;
            line.Op2=copypoint3D;
            line.color=(*it).color;
            line.diffuse=(*it).diffuseReflection;
            line.reflectionCoefficient=(*it).reflectionCoefficient;
            line.specular=(*it).specularReflection;
            lines.push_back(line);
        }
    }
    return lines;
}

img::EasyImage ThreeDsystemZBufferedLighting(const ini::Configuration &configuration){
    ini::Section general = configuration.operator[]("General");
    double pi=3.14159265358979323846;
    ini::Entry size = general.operator[]("size");
    ini::Entry backgroundcolor = general.operator[]("backgroundcolor");
    ini::Entry nrFigures = general.operator[]("nrFigures");
    ini::Entry eye = general.operator[]("eye");
    ini::Entry clipping = general.operator[]("clipping");
    ini::Entry viewDirection = general.operator[]("viewDirection");
    ini::Entry hfov = general.operator[]("hfov");
    ini::Entry aspectRatio = general.operator[]("aspectRatio");
    ini::Entry dNear = general.operator[]("dNear");
    ini::Entry dFar = general.operator[]("dFar");
    ini::Entry nrLights = general.operator[]("nrLights");
    ini::Entry shadowEnabled = general.operator[]("shadowEnabled");
    ini::Entry shadowMask = general.operator[]("shadowMask");

    int sizeParsed;
    ini::DoubleTuple backgroundcolorParsed;
    int nrFiguresParsed;
    ini::DoubleTuple eyeParsed;
    bool clippingParsed= false;
    ini::DoubleTuple viewDirectionParsed={0,0,0};
    double hfovParsed;
    double aspectRatioParsed;
    double dNearParsed;
    double dFarParsed;
    double fractalScaleParsed;
    int nrIterationsParsed;
    int nrLightsParsed=0;
    bool fractal = false;
    bool MengerSponge = false;
    bool shadowEnabledParsed=false;
    int shadowMaskParsed=0;
    bool ThickLine=false;

    size.as_int_if_exists(sizeParsed);
    backgroundcolor.as_double_tuple_if_exists(backgroundcolorParsed);
    nrFigures.as_int_if_exists(nrFiguresParsed);
    eye.as_double_tuple_if_exists(eyeParsed);
    clipping.as_bool_if_exists(clippingParsed);
    bool viewDirectionExists = viewDirection.as_double_tuple_if_exists(viewDirectionParsed);
    hfov.as_double_if_exists(hfovParsed);
    aspectRatio.as_double_if_exists(aspectRatioParsed);
    dNear.as_double_if_exists(dNearParsed);
    dFar.as_double_if_exists(dFarParsed);
    nrLights.as_int_if_exists(nrLightsParsed);
    shadowEnabled.as_bool_if_exists(shadowEnabledParsed);
    shadowMask.as_int_if_exists(shadowMaskParsed);

    if (!viewDirectionExists){
        viewDirectionParsed[0]=-eyeParsed[0];
        viewDirectionParsed[1]=-eyeParsed[1];
        viewDirectionParsed[2]=-eyeParsed[2];
    }

    vector<ThreeDfigures> figures;
    vector<Light> lights;

    for(int i=0; i!=nrLightsParsed; i++){
        Light lightParsed;
        ini::Section light = configuration.operator[]("Light" + to_string(i));
        ini::Entry infinity = light.operator[]("infinity");
        ini::Entry location = light.operator[]("location");
        ini::Entry direction = light.operator[]("direction");
        ini::Entry ambientLight = light.operator[]("ambientLight");
        ini::Entry diffuseLight = light.operator[]("diffuseLight");
        ini::Entry specularLight = light.operator[]("specularLight");
        ini::Entry spotAngle = light.operator[]("spotAngle");

        bool infinityParsed;

        infinity.as_bool_if_exists(infinityParsed);
        location.as_double_tuple_if_exists(lightParsed.location);
        direction.as_double_tuple_if_exists(lightParsed.direction);
        ambientLight.as_double_tuple_if_exists(lightParsed.ambientLight);
        diffuseLight.as_double_tuple_if_exists(lightParsed.diffuseLight);
        specularLight.as_double_tuple_if_exists(lightParsed.specularLight);
        spotAngle.as_double_if_exists(lightParsed.spotAngle);
        lightParsed.infinity=infinityParsed;
        lights.push_back(lightParsed);
    }

    vector<double> InvalidParsed={-1,-1,-1};
    bool ambient=false;
    bool diffuse=false;
    bool specular=false;
    if(!lights.empty()){
        Light tempObject =*lights.begin();
        if(lights.begin()->diffuseLight==InvalidParsed&&lights.begin()->specularLight==InvalidParsed&&lights.begin()->ambientLight!=InvalidParsed){
            ambient = true;
        } else if (lights.begin()->specularLight==InvalidParsed){
            diffuse = true;
        } else {
            specular = true;
        }
    }

    for(int count=0; count!=nrFiguresParsed; count++){
        ThreeDfigures figureParsed;
        ini::Section Figure = configuration.operator[]("Figure" + to_string(count));
        ini::Entry type = Figure.operator[]("type");
        ini::Entry rotateX = Figure.operator[]("rotateX");
        ini::Entry rotateY = Figure.operator[]("rotateY");
        ini::Entry rotateZ = Figure.operator[]("rotateZ");
        ini::Entry scale = Figure.operator[]("scale");
        ini::Entry center = Figure.operator[]("center");
        ini::Entry color = Figure.operator[]("color");
        ini::Entry ambientReflection = Figure.operator[]("ambientReflection");
        ini::Entry diffuseReflection = Figure.operator[]("diffuseReflection");
        ini::Entry specularReflection = Figure.operator[]("specularReflection");
        ini::Entry reflectionCoefficient = Figure.operator[]("reflectionCoefficient");

        std::string typeParsed;
        double rotateXParsed;
        double rotateYParsed;
        double rotateZParsed;
        double scaleParsed;
        ini::DoubleTuple centerParsed;
        ini::DoubleTuple colorParsed={0,0,0};
        int nrPointsParsed;
        int nrLinesParsed;

        type.as_string_if_exists(typeParsed);
        color.as_double_tuple_if_exists(colorParsed);
        rotateX.as_double_if_exists(rotateXParsed);
        rotateY.as_double_if_exists(rotateYParsed);
        rotateZ.as_double_if_exists(rotateZParsed);
        scale.as_double_if_exists(scaleParsed);
        center.as_double_tuple_if_exists(centerParsed);
        if(typeParsed=="ThickLineDrawing" || typeParsed=="ThickCube" || typeParsed=="ThickOctahedron" || typeParsed=="ThickDodecahedron" || typeParsed=="ThickIcosahedron" || typeParsed=="ThickTetrahedron" || typeParsed=="Thick3DLSystem" || typeParsed=="ThickBuckyBall"){
            ThickLine=true;
            typeParsed=typeParsed.substr(5,string::npos);
        }

        Lichamen3D body;
        if (typeParsed=="Cube"){
            figureParsed = body.createCube();
        } else if (typeParsed=="Tetrahedron"){
            figureParsed = body.createTetrahedron();
        } else if (typeParsed=="Octahedron"){
            figureParsed = body.createOctahedron();
        } else if (typeParsed=="Icosahedron"){
            figureParsed = body.createIcosahedron();
        } else if (typeParsed=="Dodecahedron"){
            figureParsed = body.createDodecahedron();
        } else if (typeParsed=="Cylinder"){
            ini::Entry n = Figure.operator[]("n");
            ini::Entry h = Figure.operator[]("height");
            int nParsed;
            double hParsed;
            n.as_int_if_exists(nParsed);
            h.as_double_if_exists(hParsed);
            figureParsed = body.createCylinderZ(n,hParsed);
        } else if (typeParsed=="Cone"){
            ini::Entry n = Figure.operator[]("n");
            ini::Entry h = Figure.operator[]("height");
            int nParsed;
            double hParsed;
            n.as_int_if_exists(nParsed);
            h.as_double_if_exists(hParsed);
            figureParsed = body.createConeZ(n,hParsed);
        } else if (typeParsed=="Sphere"){
            ini::Entry n = Figure.operator[]("n");
            int nParsed;
            n.as_int_if_exists(nParsed);
            figureParsed = body.createSphere(nParsed);
        } else if (typeParsed=="Torus"){
            ini::Entry r = Figure.operator[]("r");
            double rParsed;
            r.as_double_if_exists(rParsed);
            ini::Entry R = Figure.operator[]("R");
            double RParsed;
            R.as_double_if_exists(RParsed);
            ini::Entry m = Figure.operator[]("m");
            int mParsed;
            m.as_int_if_exists(mParsed);
            ini::Entry n = Figure.operator[]("n");
            int nParsed;
            n.as_int_if_exists(nParsed);
            figureParsed = body.createTorus(rParsed, RParsed, mParsed, nParsed);
        } else if (typeParsed=="3DLSystem"){
            ini::Entry inputfile3D = Figure.operator[]("inputfile");
            string inputfile3DParsed;
            inputfile3D.as_string_if_exists(inputfile3DParsed);
            figureParsed = Lsystem3D(inputfile3DParsed);
        } else if (typeParsed=="FractalCube"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createCube();
        } else if (typeParsed=="BuckyBall"){
            figureParsed = body.createBuckyBall();
        } else if (typeParsed=="FractalBuckyBall"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createBuckyBall();
        } else if (typeParsed=="FractalDodecahedron"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createDodecahedron();
        }else if (typeParsed=="FractalIcosahedron"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createIcosahedron();
        } else if (typeParsed=="FractalOctahedron"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createOctahedron();
        } else if (typeParsed=="FractalTetrahedron"){
            fractal=true;
            ini::Entry fractalScale = Figure.operator[]("fractalScale");
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            fractalScale.as_double_if_exists(fractalScaleParsed);
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createTetrahedron();
        } else if (typeParsed=="MengerSponge"){
            MengerSponge = true;
            ini::Entry nrIterations = Figure.operator[]("nrIterations");
            nrIterations.as_int_if_exists(nrIterationsParsed);
            figureParsed = body.createCube();
        } else {
            ini::Entry nrPoints = Figure.operator[]("nrPoints");
            ini::Entry nrLines = Figure.operator[]("nrLines");

            nrPoints.as_int_if_exists(nrPointsParsed);
            nrLines.as_int_if_exists(nrLinesParsed);

            for (int count2 = 0; count2 != nrPointsParsed; count2++) {
                ini::Entry point = Figure.operator[]("point" + to_string(count2));
                ini::DoubleTuple pointParsed;
                point.as_double_tuple_if_exists(pointParsed);
                Point3D point_3D;
                point_3D.x = pointParsed[0];
                point_3D.y = pointParsed[1];
                point_3D.z = pointParsed[2];
                figureParsed.points.push_back(point_3D);
            }

            for (int count3 = 0; count3 != nrLinesParsed; count3++) {
                ini::Entry line = Figure.operator[]("line" + to_string(count3));
                ini::DoubleTuple lineParsed;
                line.as_double_tuple_if_exists(lineParsed);
                Face3D face;
                for (ini::DoubleTuple::iterator it = lineParsed.begin(); it != lineParsed.end(); it++) {
                    face.point_indexes.push_back((*it));
                }
                figureParsed.faces.push_back(face);
            }
        }
        if(!ambient&&!specular&&!diffuse){
            figureParsed.color=img::Color(colorParsed[0]*255,colorParsed[1]*255,colorParsed[2]*255);
        } else {
            ambientReflection.as_double_tuple_if_exists(figureParsed.ambientReflection);
            diffuseReflection.as_double_tuple_if_exists(figureParsed.diffuseReflection);
            specularReflection.as_double_tuple_if_exists(figureParsed.specularReflection);
            reflectionCoefficient.as_double_if_exists(figureParsed.reflectionCoefficient);
        }
        transform(figureParsed, Scale(scaleParsed));
        if (rotateXParsed != 0){
            transform(figureParsed, RotationX(rotateXParsed*pi/180));
        }
        if (rotateYParsed != 0){
            transform(figureParsed, RotationY(rotateYParsed*pi/180));
        }
        if (rotateZParsed != 0){
            transform(figureParsed, RotationZ(rotateZParsed*pi/180));
        }
        transform(figureParsed, Translation(centerParsed[0],centerParsed[1],centerParsed[2]));
        Figures3D tempfigures;
        if (fractal){
            tempfigures.push_back(figureParsed);
            generatefractal(tempfigures,nrIterationsParsed,fractalScaleParsed);
            figures.insert(figures.end(),tempfigures.begin(),tempfigures.end());
        } else if(MengerSponge){
            tempfigures.push_back(figureParsed);
            generateMengerSponge(tempfigures,nrIterationsParsed);
            figures.insert(figures.end(),tempfigures.begin(),tempfigures.end());
        } else if (ThickLine){
            ini::Entry r = Figure.operator[]("radius");
            double rParsed;
            r.as_double_if_exists(rParsed);
            ini::Entry m = Figure.operator[]("m");
            int mParsed;
            m.as_int_if_exists(mParsed);
            ini::Entry n = Figure.operator[]("n");
            int nParsed;
            n.as_int_if_exists(nParsed);
            tempfigures.push_back(figureParsed);
            generateThickFigure(tempfigures,rParsed,nParsed,mParsed, 1);
            figures.insert(figures.end(),tempfigures.begin(),tempfigures.end());
        } else {
            figures.push_back(figureParsed);
        }
    }
    double r = sqrt((pow(eyeParsed[0],2)+pow(eyeParsed[1],2)+pow(eyeParsed[2],2)));
    double r2 = sqrt((pow(viewDirectionParsed[0],2)+pow(viewDirectionParsed[1],2)+pow(viewDirectionParsed[2],2)));
    double theta=atan2(-viewDirectionParsed[1],-viewDirectionParsed[0]);
    double phi=acos((-viewDirectionParsed[2]/r2));
    Matrix V = eyePointTrans(Vector3D::point(r,theta,phi));
    applyTransformations(figures,V);

    if (clippingParsed){
        for(Figures3D::iterator it=figures.begin(); it!=figures.end(); it++){
            (*it).faces = triangulate((*it).faces);
        }
        double dval;
        double hfovRadian = hfovParsed*(pi/180);
        for(int k=0; k!=6;k++) {
            for (vector<ThreeDfigures>::iterator it = figures.begin(); it != figures.end(); it++) {
                switch (k) {
                    case 0:
                        dval = -dNearParsed;
                        break;
                    case 1:
                        dval = -dFarParsed;
                        break;
                    case 2:
                        dval = dNearParsed * tan(hfovRadian / 2);
                        break;
                    case 3:
                        dval = -dNearParsed * tan(hfovRadian / 2);
                        break;
                    case 4:
                        dval = (dNearParsed * tan(hfovRadian / 2)) / aspectRatioParsed;
                        break;
                    case 5:
                        dval = (-dNearParsed * tan(hfovRadian / 2)) / aspectRatioParsed;
                        break;
                }
                map<int, Point3D> outside;
                map<int, Point3D> outsideALL;
                int count = 0;
                int zeroChanger = (*it).points.size();
                for (vector<Point3D>::iterator it2 = (*it).points.begin(); it2 != (*it).points.end(); it2++, count++) {
                    switch (k) {
                        case 0:
                            if (it2->z > dval) {
                                outside[count] = (*it2);
                            }
                            break;
                        case 1:
                            if (it2->z < dval) {
                                outside[count] = (*it2);
                            }
                            break;
                        case 2:
                            if ((-it2->x * dNearParsed / it2->z) > dval) {
                                outside[count] = (*it2);
                            }
                            break;
                        case 3:
                            if ((-it2->x * dNearParsed / it2->z) < dval) {
                                outside[count] = (*it2);
                            }
                            break;
                        case 4:
                            if ((-it2->y * dNearParsed / it2->z) > dval) {
                                outside[count] = (*it2);
                            }
                            break;
                        case 5:
                            if ((-it2->y * dNearParsed / it2->z) < dval) {
                                outside[count] = (*it2);
                            }
                            break;
                    }
                }
                for (map<int, Point3D>::iterator it2 = outside.begin(); it2 != outside.end(); it2++) {
                    outsideALL[it2->first] = *((*it).points.begin() + it2->first);
                }
                for (vector<Face3D>::iterator it3 = (*it).faces.begin(); it3 != (*it).faces.end(); it3++) {
                    for (vector<int>::iterator it4 = (*it3).point_indexes.begin();
                         it4 != (*it3).point_indexes.end(); it4++) {
                        if (outside.find(*it4) != outside.end()) {
                            if ((*it4) == 0) {
                                (*it4) = -zeroChanger;
                            } else {
                                (*it4) = -(*it4);
                            }
                        }
                    }
                }
                int originalPointsSize = (*it).points.size();
                for (map<int, Point3D>::iterator pointTOBD = outside.begin(); pointTOBD != outside.end(); pointTOBD++) {
                    (*it).points.erase(find((*it).points.begin(), (*it).points.end(), pointTOBD->second));
                }
                map<int, int> afgetrokken;
                int currentIterate = 0;
                for (int j = 0; j != originalPointsSize; j++) {
                    for (map<int, Point3D>::iterator pointTOBD = outside.begin();
                         pointTOBD != outside.end(); pointTOBD++) {
                        if (pointTOBD == outside.begin() && currentIterate != 0) {
                            pointTOBD = outside.find(currentIterate);
                        }
                        if (currentIterate != 0) {
                            afgetrokken[j] += afgetrokken[j - 1];
                            currentIterate = 0;
                        }
                        if (pointTOBD->first < j) {
                            afgetrokken[j]++;
                        } else {
                            currentIterate = pointTOBD->first;
                            break;
                        }
                    }
                }
                for (vector<Face3D>::iterator it3 = (*it).faces.begin(); it3 != (*it).faces.end(); it3++) {
                    for (vector<int>::iterator it4 = (*it3).point_indexes.begin();
                         it4 != (*it3).point_indexes.end(); it4++) {
                        (*it4) = (*it4) - afgetrokken[*it4];
                    }
                }
                vector<Face3D> copy = (*it).faces;
                (*it).faces.clear();
                for (vector<Face3D>::iterator it2 = copy.begin(); it2 != copy.end(); it2++) {
                    Point3D p1;
                    Point3D p2;
                    Point3D p3;
                    bool a = true;
                    bool b = true;
                    bool c = true;
                    vector<Point3D> inside;
                    vector<Point3D> outside2;
                    if (it2->point_indexes[0] >= 0) {
                        p1 = (*it).points[it2->point_indexes[0]];
                        inside.push_back(p1);
                    } else {
                        if (it2->point_indexes[0] <= -zeroChanger) {
                            p1 = outsideALL[0];
                            a = false;
                            outside2.push_back(p1);
                        } else {
                            p1 = outsideALL[-it2->point_indexes[0]];
                            a = false;
                            outside2.push_back(p1);
                        }
                    }
                    if (it2->point_indexes[1] >= 0) {
                        p2 = (*it).points[it2->point_indexes[1]];
                        inside.push_back(p2);
                    } else {
                        if (it2->point_indexes[1] <= -zeroChanger) {
                            p2 = outsideALL[0];
                            b = false;
                            outside2.push_back(p2);
                        } else {
                            p2 = outsideALL[-it2->point_indexes[1]];
                            b = false;
                            outside2.push_back(p2);
                        }
                    }
                    if (it2->point_indexes[2] >= 0) {
                        p3 = (*it).points[it2->point_indexes[2]];
                        inside.push_back(p3);
                    } else {
                        if (it2->point_indexes[2] <= -zeroChanger) {
                            p3 = outsideALL[0];
                            c = false;
                            outside2.push_back(p3);
                        } else {
                            p3 = outsideALL[-it2->point_indexes[2]];
                            c = false;
                            outside2.push_back(p3);
                        }
                    }
                    if (a && b && c) {
                        Face3D temp;
                        temp.point_indexes = {it2->point_indexes[0], it2->point_indexes[1], it2->point_indexes[2]};
                        (*it).faces.push_back(temp);
                    } else if ((!a) && (!b) && (!c)) {

                    } else {
                        if (inside.size() == 1) {
                            int position;
                            if (a) {
                                position = it2->point_indexes[0];
                            } else if (b) {
                                position = it2->point_indexes[1];
                            } else {
                                position = it2->point_indexes[2];
                            }
                            Point3D p;
                            Point3D p_2;
                            double Pz;
                            switch (k) {
                                case 0:
                                case 1:
                                    Pz = ((dval - inside[0].z) / (outside2[0].z - inside[0].z));
                                    p.z = dval;
                                    p.x = Pz * outside2[0].x + (1 - Pz) * inside[0].x;
                                    p.y = Pz * outside2[0].y + (1 - Pz) * inside[0].y;
                                    Pz = ((dval - inside[0].z) / (outside2[1].z - inside[0].z));
                                    p_2.z = dval;
                                    p_2.x = Pz * outside2[1].x + (1 - Pz) * inside[0].x;
                                    p_2.y = Pz * outside2[1].y + (1 - Pz) * inside[0].y;
                                    (*it).points.push_back(p);
                                    (*it).points.push_back(p_2);
                                    break;
                                case 2:
                                case 3:
                                    Pz = (inside[0].x * dNearParsed + inside[0].z * dval) /
                                         ((inside[0].x - outside2[0].x) * dNearParsed +
                                          (inside[0].z - outside2[0].z) * dval);
                                    p.z = Pz * outside2[0].z + (1 - Pz) * inside[0].z;
                                    p.x = -dval * p.z / dNearParsed;
                                    p.y = Pz * outside2[0].y + (1 - Pz) * inside[0].y;
                                    Pz = (inside[0].x * dNearParsed + inside[0].z * dval) /
                                         ((inside[0].x - outside2[1].x) * dNearParsed +
                                          (inside[0].z - outside2[1].z) * dval);
                                    p_2.z = Pz * outside2[1].z + (1 - Pz) * inside[0].z;
                                    p_2.x = -dval * p_2.z / dNearParsed;
                                    p_2.y = Pz * outside2[1].y + (1 - Pz) * inside[0].y;
                                    (*it).points.push_back(p);
                                    (*it).points.push_back(p_2);
                                    break;
                                case 4:
                                case 5:
                                    Pz = (inside[0].y * dNearParsed + inside[0].z * dval) /
                                         ((inside[0].y - outside2[0].y) * dNearParsed +
                                          (inside[0].z - outside2[0].z) * dval);
                                    p.z = Pz * outside2[0].z + (1 - Pz) * inside[0].z;
                                    p.x = Pz * outside2[0].x + (1 - Pz) * inside[0].x;
                                    p.y = -dval * p.z / dNearParsed;
                                    Pz = (inside[0].y * dNearParsed + inside[0].z * dval) /
                                         ((inside[0].y - outside2[1].y) * dNearParsed +
                                          (inside[0].z - outside2[1].z) * dval);
                                    p_2.z = Pz * outside2[1].z + (1 - Pz) * inside[0].z;
                                    p_2.x = Pz * outside2[1].x + (1 - Pz) * inside[0].x;
                                    p_2.y = -dval * p_2.z / dNearParsed;
                                    (*it).points.push_back(p);
                                    (*it).points.push_back(p_2);
                                    break;
                            }
                            Face3D face;
                            face.point_indexes = {static_cast<int>((*it).points.size() - 2),
                                                  static_cast<int>((*it).points.size() - 1), position};
                            (*it).faces.push_back(face);
                        } else {
                            int position;
                            int position2;
                            if (a && b) {
                                position = it2->point_indexes[0];
                                position2 = it2->point_indexes[1];
                            } else if (a && c) {
                                position = it2->point_indexes[0];
                                position2 = it2->point_indexes[2];
                            } else {
                                position = it2->point_indexes[1];
                                position2 = it2->point_indexes[2];
                            }
                            Point3D p;
                            Point3D p_2;
                            double Pz;
                            switch (k) {
                                case 0:
                                case 1:
                                    Pz = ((dval - inside[0].z) / (outside2[0].z - inside[0].z));
                                    p.z = dval;
                                    p.x = Pz * outside2[0].x + (1 - Pz) * inside[0].x;
                                    p.y = Pz * outside2[0].y + (1 - Pz) * inside[0].y;
                                    Pz = ((dval - inside[1].z) / (outside2[0].z - inside[1].z));
                                    p_2.z = dval;
                                    p_2.x = Pz * outside2[0].x + (1 - Pz) * inside[1].x;
                                    p_2.y = Pz * outside2[0].y + (1 - Pz) * inside[1].y;
                                    (*it).points.push_back(p);
                                    (*it).points.push_back(p_2);
                                    break;
                                case 2:
                                case 3:
                                    Pz = (inside[0].x * dNearParsed + inside[0].z * dval) /
                                         ((inside[0].x - outside2[0].x) * dNearParsed +
                                          (inside[0].z - outside2[0].z) * dval);
                                    p.z = Pz * outside2[0].z + (1 - Pz) * inside[0].z;
                                    p.x = -dval * p.z / dNearParsed;
                                    p.y = Pz * outside2[0].y + (1 - Pz) * inside[0].y;
                                    Pz = (inside[1].x * dNearParsed + inside[1].z * dval) /
                                         ((inside[1].x - outside2[0].x) * dNearParsed +
                                          (inside[1].z - outside2[0].z) * dval);
                                    p_2.z = Pz * outside2[0].z + (1 - Pz) * inside[1].z;
                                    p_2.x = -dval * p_2.z / dNearParsed;
                                    p_2.y = Pz * outside2[0].y + (1 - Pz) * inside[1].y;
                                    (*it).points.push_back(p);
                                    (*it).points.push_back(p_2);
                                    break;
                                case 4:
                                case 5:
                                    Pz = (inside[0].y * dNearParsed + inside[0].z * dval) /
                                         ((inside[0].y - outside2[0].y) * dNearParsed +
                                          (inside[0].z - outside2[0].z) * dval);
                                    p.z = Pz * outside2[0].z + (1 - Pz) * inside[0].z;
                                    p.x = Pz * outside2[0].x + (1 - Pz) * inside[0].x;
                                    p.y = -dval * p.z / dNearParsed;
                                    Pz = (inside[1].y * dNearParsed + inside[1].z * dval) /
                                         ((inside[1].y - outside2[0].y) * dNearParsed +
                                          (inside[1].z - outside2[0].z) * dval);
                                    p_2.z = Pz * outside2[0].z + (1 - Pz) * inside[1].z;
                                    p_2.x = Pz * outside2[0].x + (1 - Pz) * inside[1].x;
                                    p_2.y = -dval * p_2.z / dNearParsed;
                                    (*it).points.push_back(p);
                                    (*it).points.push_back(p_2);
                                    break;
                            }
                            Face3D face;
                            Face3D face_2;
                            face.point_indexes = {static_cast<int>((*it).points.size() - 2), position, position2};
                            face_2.point_indexes = {static_cast<int>((*it).points.size() - 2),
                                                    static_cast<int>((*it).points.size() - 1), position2};
                            (*it).faces.push_back(face);
                            (*it).faces.push_back(face_2);
                        }
                    }
                }
            }
        }
    }
    if(ambient||diffuse||specular){
        for(vector<Light>::const_iterator light=lights.begin(); light!=lights.end(); light++){
            for(vector<ThreeDfigures>::iterator figure=figures.begin(); figure!=figures.end();figure++){
                figure->color.red+=round((light->ambientLight[0]*figure->ambientReflection[0])*255);
                figure->color.green+=round((light->ambientLight[1]*figure->ambientReflection[1])*255);
                figure->color.blue+=round((light->ambientLight[2]*figure->ambientReflection[2])*255);
            }
        }
    }
    Lines2D lines = doProjectionZBufferingcolor(figures);
    double xMax=0;
    double xMin=0;
    double yMax=0;
    double yMin=0;
    for(std::vector<Line2D>::iterator it=lines.begin();it!=lines.end();it++){
        vector<Point2D> p;
        p.push_back(it->p1);
        p.push_back(it->p2);
        for(int i=0; i!=2; i++){
            if (xMin>p[i].x){
                xMin=p[i].x;
            } else if(xMax<p[i].x){
                xMax=p[i].x;
            }
            if (yMin>p[i].y){
                yMin=p[i].y;
            } else if(yMax<p[i].y){
                yMax=p[i].y;
            }
        }
    }
    double xRange = std::abs(xMax) + std::abs(xMin);
    double yRange = std::abs(yMax) + std::abs(yMin);
    double xImage=sizeParsed*(xRange/ std::max(xRange,yRange));
    double yImage=sizeParsed*(yRange/ std::max(xRange,yRange));
    double d=0.95*(xImage/xRange);
    double dx = (xImage/2) - d*((xMin + xMax)/2);
    double dy = (yImage/2) - d*((yMin + yMax)/2);
    img::EasyImage image(std::round(xImage),std::round(yImage));
    img::Color backgroundcolorDone(backgroundcolorParsed[0]*255,backgroundcolorParsed[1]*255,backgroundcolorParsed[2]*255);
    ZBuffer Z(std::round(xImage),std::round(yImage),backgroundcolorDone);
    vector<Line2D> lines2;
    Matrix Vcopy=V;
    Vcopy.inv();
    if(shadowEnabledParsed){
        vector<vector<Line2D>> shadow_lines;
        for(vector<Light>::iterator light=lights.begin(); light!=lights.end(); light++){
            double rs=sqrt(pow(light->location[0],2)+pow(light->location[1],2)+pow(light->location[2],2));
            double thetas=atan2(light->location[1],light->location[0]);
            double phis=acos((light->location[2]/rs));
            Matrix Vs = eyePointTrans(Vector3D::point(rs,thetas,phis));
            light->V_shadow=Vs;
            vector<ThreeDfigures> copy2=figures;
            applyTransformations(copy2,Vcopy);
            applyTransformations(copy2,Vs);
            vector<Line2D> shadow_line=doProjectionZBufferingcolor(copy2);
            double xMaxS=0;
            double xMinS=0;
            double yMaxS=0;
            double yMinS=0;
            for(std::vector<Line2D>::iterator it=shadow_line.begin();it!=shadow_line.end();it++){
                vector<Point2D> p;
                p.push_back(it->p1);
                p.push_back(it->p2);
                for(int i=0; i!=2; i++){
                    if (xMinS>p[i].x){
                        xMinS=p[i].x;
                    } else if(xMaxS<p[i].x){
                        xMaxS=p[i].x;
                    }
                    if (yMinS>p[i].y){
                        yMinS=p[i].y;
                    } else if(yMaxS<p[i].y){
                        yMaxS=p[i].y;
                    }
                }
            }
            double xRangeS = std::abs(xMaxS) + std::abs(xMinS);
            double yRangeS = std::abs(yMaxS) + std::abs(yMinS);
            double xImageS=shadowMaskParsed*(xRangeS/ std::max(xRangeS,yRangeS));
            double yImageS=shadowMaskParsed*(yRangeS/ std::max(xRangeS,yRangeS));
            double dS=0.95*(xImageS/xRangeS);
            double dxS = (xImageS/2) - dS*((xMinS + xMaxS)/2);
            double dyS = (yImageS/2) - dS*((yMinS + yMaxS)/2);
            light->dSdxSdy={dS,dxS,dyS};
            img::Color black(0,0,0);
            ZBuffer Zs(round(xImageS),round(yImageS),black);
            for(std::vector<Line2D>::iterator it=shadow_line.begin();it!=shadow_line.end();it++){
                Point2D p1;
                if((*it).Op1.z==0){
                    p1.x=dS*(-(*it).Op1.x)+dxS;
                    p1.y=dS*(-(*it).Op1.y)+dyS;
                } else {
                    p1.x=((dS*(-(*it).Op1.x))/(*it).Op1.z)+dxS;
                    p1.y=((dS*(-(*it).Op1.y))/(*it).Op1.z)+dyS;
                }
                Point3D originalp1 = (*it).Op1;
                Point2D p2;
                if((*it).Op2.z==0){
                    p2.x=dS*(-(*it).Op2.x)+dxS;
                    p2.y=dS*(-(*it).Op2.y)+dyS;
                } else {
                    p2.x=((dS*(-(*it).Op2.x))/(*it).Op2.z)+dxS;
                    p2.y=((dS*(-(*it).Op2.y))/(*it).Op2.z)+dyS;
                }
                Point3D originalp2 = (*it).Op2;
                Line2D temp_line=(*it);
                it++;
                Point2D p3 = (*it).p2;
                if((*it).Op2.z==0){
                    p3.x=dS*(-(*it).Op2.x)+dxS;
                    p3.y=dS*(-(*it).Op2.y)+dyS;
                } else {
                    p3.x=(dS*(-(*it).Op2.x)/(*it).Op2.z)+dxS;
                    p3.y=(dS*(-(*it).Op2.y)/(*it).Op2.z)+dyS;
                }
                Point3D originalp3 = (*it).Op2;
                Line2D temp_line2=(*it);
                it++;
                vector<Point2D> temp={p1,p2,p1,p3,p2,p3};
                double max_PRE1=max(p1.y,p2.y);
                double max_PRE2=max(max_PRE1,p3.y);
                int max = round(max_PRE2+0.5);
                double min_PRE1=min(p1.y,p2.y);
                double min_PRE2=min(min_PRE1,p3.y);
                int min = round(min_PRE2-0.5);

                Point2D G;
                G.x=(p1.x + p2.x + p3.x)/3;
                G.y=(p1.y + p2.y + p3.y)/3;

                double Gz;
                Gz=(1/(3*originalp1.z)) + (1/(3*originalp2.z)) + (1/(3*originalp3.z));

                Vector3D A = Vector3D::point(originalp1.x,originalp1.y,originalp1.z);
                Vector3D B = Vector3D::point(originalp2.x,originalp2.y,originalp2.z);
                Vector3D C = Vector3D::point(originalp3.x,originalp3.y,originalp3.z);
                Vector3D u = B-A;
                Vector3D v = C-A;
                Vector3D w = Vector3D::cross(u,v);

                for(int i=min;i!=max;i++){
                    int count=0;
                    double ABL = std::numeric_limits<double>::infinity();
                    double ACL = std::numeric_limits<double>::infinity();
                    double BCL = std::numeric_limits<double>::infinity();
                    double ABR = -std::numeric_limits<double>::infinity();
                    double ACR = -std::numeric_limits<double>::infinity();
                    double BCR = -std::numeric_limits<double>::infinity();
                    bool next=true;
                    for (vector<Point2D>::const_iterator it2=temp.begin(); it2!=temp.end(); it2++, count++){
                        Point2D p=(*it2);
                        it2++;
                        Point2D q=(*it2);
                        if ((i-p.y)*(i-q.y)<=0 && p.y!=q.y){
                            if (count==0){
                                next=false;
                                ABL = abs(q.x + (p.x-q.x)*((i-q.y)/(p.y-q.y)));
                                ABR = abs(q.x + (p.x-q.x)*((i-q.y)/(p.y-q.y)));
                            } else if (count==1){
                                next=false;
                                ACL = abs(q.x + (p.x-q.x)*((i-q.y)/(p.y-q.y)));
                                ACR = abs(q.x + (p.x-q.x)*((i-q.y)/(p.y-q.y)));
                            } else {
                                next=false;
                                BCL = abs(q.x + (p.x-q.x)*((i-q.y)/(p.y-q.y)));
                                BCR = abs(q.x + (p.x-q.x)*((i-q.y)/(p.y-q.y)));
                            }
                        }
                    }
                    if(next){
                        continue;
                    }
                    double max_PRE11=std::max(ABR,ACR);
                    double max_PRE22=std::max(max_PRE11,BCR);
                    int xr = round(max_PRE22+0.5);
                    double min_PRE11=std::min(ABL,ACL);
                    double min_PRE22=std::min(min_PRE11,BCL);
                    int xl = round(min_PRE22-0.5);

                    double k = w.x*originalp1.x + w.y*originalp1.y + w.z*originalp1.z;

                    double zx=w.x/(-dS*k);
                    double zy=w.y/(-dS*k);

                    for (int j=xl+1; j!=xr;j++){
                        double z = Gz + (j-G.x)*zx + (i-G.y)*zy;
                        if(Zs.buffer[std::round(j)][std::round(i)].first>z){
                            Zs.buffer[std::round(j)][std::round(i)].first=z;
                            Zs.buffer[std::round(j)][std::round(i)].second=(*it).color;
                        }
                    }
                }
            }
            light->shadow_buffer=Zs;
        }
    }


    for(std::vector<Line2D>::iterator it=lines.begin();it!=lines.end();it++){
        Point2D p1;
        if((*it).Op1.z==0){
            p1.x=d*(-(*it).Op1.x)+dx;
            p1.y=d*(-(*it).Op1.y)+dy;
        } else {
            p1.x=((d*(-(*it).Op1.x))/(*it).Op1.z)+dx;
            p1.y=((d*(-(*it).Op1.y))/(*it).Op1.z)+dy;
        }
        Point3D originalp1 = (*it).Op1;
        Point2D p2;
        if((*it).Op2.z==0){
            p2.x=d*(-(*it).Op2.x)+dx;
            p2.y=d*(-(*it).Op2.y)+dy;
        } else {
            p2.x=((d*(-(*it).Op2.x))/(*it).Op2.z)+dx;
            p2.y=((d*(-(*it).Op2.y))/(*it).Op2.z)+dy;
        }
        Point3D originalp2 = (*it).Op2;
        Line2D temp_line=(*it);
        it++;
        Point2D p3 = (*it).p2;
        if((*it).Op2.z==0){
            p3.x=d*(-(*it).Op2.x)+dx;
            p3.y=d*(-(*it).Op2.y)+dy;
        } else {
            p3.x=(d*(-(*it).Op2.x)/(*it).Op2.z)+dx;
            p3.y=(d*(-(*it).Op2.y)/(*it).Op2.z)+dy;
        }
        Point3D originalp3 = (*it).Op2;
        Line2D temp_line2=(*it);
        it++;
        vector<Point2D> temp={p1,p2,p1,p3,p2,p3};
        double max_PRE1=max(p1.y,p2.y);
        double max_PRE2=max(max_PRE1,p3.y);
        int max = round(max_PRE2+0.5);
        double min_PRE1=min(p1.y,p2.y);
        double min_PRE2=min(min_PRE1,p3.y);
        int min = round(min_PRE2-0.5);

        Point2D G;
        G.x=(p1.x + p2.x + p3.x)/3;
        G.y=(p1.y + p2.y + p3.y)/3;

        double Gz;
        Gz=(1/(3*originalp1.z)) + (1/(3*originalp2.z)) + (1/(3*originalp3.z));

        Vector3D A = Vector3D::point(originalp1.x,originalp1.y,originalp1.z);
        Vector3D B = Vector3D::point(originalp2.x,originalp2.y,originalp2.z);
        Vector3D C = Vector3D::point(originalp3.x,originalp3.y,originalp3.z);
        Vector3D u = B-A;
        Vector3D v = C-A;
        Vector3D w = Vector3D::cross(u,v);
        Vector3D normaalvector = w/w.length();

        if(clippingParsed){
            if((w.x*originalp1.x + w.y*originalp1.y + w.z*originalp1.z)>0){
                normaalvector=-normaalvector;
            }
        }

        if(diffuse){
            for(vector<Light>::iterator light=lights.begin(); light!=lights.end(); light++){
                if(light->infinity){
                    Vector3D ld = Vector3D::vector(light->direction[0],light->direction[1],light->direction[2]);
                    Vector3D l = -(ld/ld.length());
                    l=l*V;
                    double a = Vector3D::dot(normaalvector,l);
                    if (a<0){
                        continue;
                    }
                    if((*it).color.red+round((*it).diffuse[0]*light->diffuseLight[0]*a*255)>=255){
                        (*it).color.red=255;
                    } else {
                        (*it).color.red+=round((*it).diffuse[0]*light->diffuseLight[0]*a*255);
                    }
                    if((*it).color.green+round((*it).diffuse[1]*light->diffuseLight[1]*a*255)>=255){
                        (*it).color.green=255;
                    } else {
                        (*it).color.green+=round((*it).diffuse[1]*light->diffuseLight[1]*a*255);
                    }
                    if((*it).color.blue+round((*it).diffuse[2]*light->diffuseLight[2]*a*255)>=255){
                        (*it).color.blue=255;
                    } else {
                        (*it).color.blue+=round((*it).diffuse[2]*light->diffuseLight[2]*a*255);
                    }
                }
            }
        }

        for(int i=min;i!=max;i++){
            int count=0;
            double ABL = std::numeric_limits<double>::infinity();
            double ACL = std::numeric_limits<double>::infinity();
            double BCL = std::numeric_limits<double>::infinity();
            double ABR = -std::numeric_limits<double>::infinity();
            double ACR = -std::numeric_limits<double>::infinity();
            double BCR = -std::numeric_limits<double>::infinity();
            bool next=true;
            for (vector<Point2D>::const_iterator it2=temp.begin(); it2!=temp.end(); it2++, count++){
                Point2D p=(*it2);
                it2++;
                Point2D q=(*it2);
                if ((i-p.y)*(i-q.y)<=0 && p.y!=q.y){
                    if (count==0){
                        next=false;
                        ABL = abs(q.x + (p.x-q.x)*((i-q.y)/(p.y-q.y)));
                        ABR = abs(q.x + (p.x-q.x)*((i-q.y)/(p.y-q.y)));
                    } else if (count==1){
                        next=false;
                        ACL = abs(q.x + (p.x-q.x)*((i-q.y)/(p.y-q.y)));
                        ACR = abs(q.x + (p.x-q.x)*((i-q.y)/(p.y-q.y)));
                    } else {
                        next=false;
                        BCL = abs(q.x + (p.x-q.x)*((i-q.y)/(p.y-q.y)));
                        BCR = abs(q.x + (p.x-q.x)*((i-q.y)/(p.y-q.y)));
                    }
                }
            }
            if(next){
                continue;
            }
            double max_PRE11=std::max(ABR,ACR);
            double max_PRE22=std::max(max_PRE11,BCR);
            int xr = round(max_PRE22+0.5);
            double min_PRE11=std::min(ABL,ACL);
            double min_PRE22=std::min(min_PRE11,BCL);
            int xl = round(min_PRE22-0.5);

            double k = w.x*originalp1.x + w.y*originalp1.y + w.z*originalp1.z;

            double zx=w.x/(-d*k);
            double zy=w.y/(-d*k);

            for (int j=xl+1; j!=xr;j++){
                double z = Gz + (j-G.x)*zx + (i-G.y)*zy;
                if(Z.buffer[j][i].first>z){
                    Z.buffer[j][i].first=z;
                    bool diffuseSucces=false;
                    bool morelights=false;
                    for(vector<Light>::iterator light=lights.begin(); light!=lights.end(); light++){
                        if (!light->infinity&&!specular&&diffuse){
                            img::Color tempcolor=(*it).color;
                            Vector3D locationPoint = Vector3D::point(light->location[0],light->location[1],light->location[2]);
                            locationPoint=locationPoint*V;
                            Vector3D planePoint = Vector3D::point((((1/z)*(j-dx))/-d),(((1/z)*(i-dy))/-d),1/z);
                            Vector3D ldp = planePoint-locationPoint;
                            Vector3D lp = -(ldp/ldp.length());
                            double a;
                            double as;
                            if(light->spotAngle==-1){
                                a = Vector3D::dot(normaalvector,lp);
                            } else {
                                as= light->spotAngle*(pi/180);
                                a = 1-(1-Vector3D::dot(normaalvector,lp))/(1-cos(as));
                            }
                            if (a<0){
                                continue;
                            }
                            if(shadowEnabledParsed){
                                Vector3D lightpoint=planePoint*Vcopy;
                                lightpoint=lightpoint*light->V_shadow;
                                lightpoint.x=((light->dSdxSdy[0]*lightpoint.x)/-lightpoint.z)+light->dSdxSdy[1];
                                lightpoint.y=((light->dSdxSdy[0]*lightpoint.y)/-lightpoint.z)+light->dSdxSdy[2];
                                double Za=light->shadow_buffer.buffer[floor(lightpoint.x)][floor(lightpoint.y)].first;
                                double Zb=light->shadow_buffer.buffer[floor(lightpoint.x)+1][floor(lightpoint.y)].first;
                                double Zc=light->shadow_buffer.buffer[floor(lightpoint.x)][floor(lightpoint.y)+1].first;
                                double Zd=light->shadow_buffer.buffer[floor(lightpoint.x)+1][floor(lightpoint.y)+1].first;
                                double ax= lightpoint.x-floor(lightpoint.x);
                                double ay= lightpoint.y-floor(lightpoint.y);
                                double Ze= (1-ax)*Za+ax*Zb;
                                double Zf= (1-ax)*Zc+ax*Zd;
                                double zlV = ay*Ze+(1-ay)*Zf;
                                if(abs(1/lightpoint.z-zlV)>0.00001 && 1/lightpoint.z<0 && 1/lightpoint.z>Za && 1/lightpoint.z>Zb && 1/lightpoint.z>Zc && 1/lightpoint.z>Zd){
                                    continue;
                                }
                            }
                            if(!morelights){
                                if(tempcolor.red+round((*it).diffuse[0]*light->diffuseLight[0]*a*255)>=255){
                                    tempcolor.red=255;
                                } else {
                                    tempcolor.red+=round((*it).diffuse[0]*light->diffuseLight[0]*a*255);
                                }
                                if(tempcolor.green+round((*it).diffuse[1]*light->diffuseLight[1]*a*255)>=255){
                                    tempcolor.green=255;
                                } else {
                                    tempcolor.green+=round((*it).diffuse[1]*light->diffuseLight[1]*a*255);
                                }
                                if(tempcolor.blue+round((*it).diffuse[2]*light->diffuseLight[2]*a*255)>=255){
                                    tempcolor.blue=255;
                                } else {
                                    tempcolor.blue+=round((*it).diffuse[2]*light->diffuseLight[2]*a*255);
                                }
                            } else {
                                if(Z.buffer[std::round(j)][std::round(i)].second.red+round((*it).diffuse[0]*light->diffuseLight[0]*a*255)>=255){
                                    tempcolor.red=255;
                                } else {
                                    tempcolor.red=round((*it).diffuse[0]*light->diffuseLight[0]*a*255+Z.buffer[std::round(j)][std::round(i)].second.red);
                                }
                                if(Z.buffer[std::round(j)][std::round(i)].second.green+round((*it).diffuse[1]*light->diffuseLight[1]*a*255)>=255){
                                    tempcolor.green=255;
                                } else {
                                    tempcolor.green=round((*it).diffuse[1]*light->diffuseLight[1]*a*255+Z.buffer[std::round(j)][std::round(i)].second.green);
                                }
                                if(Z.buffer[std::round(j)][std::round(i)].second.blue+round((*it).diffuse[2]*light->diffuseLight[2]*a*255)>=255){
                                    tempcolor.blue=255;
                                } else {
                                    tempcolor.blue=round((*it).diffuse[2]*light->diffuseLight[2]*a*255+Z.buffer[std::round(j)][std::round(i)].second.blue);
                                }
                            }
                            diffuseSucces= true;
                            Z.buffer[std::round(j)][std::round(i)].second=tempcolor;
                            morelights= true;
                        } else if(!light->infinity&&specular){
                            img::Color tempcolor=(*it).color;
                            Vector3D locationPoint = Vector3D::point(light->location[0],light->location[1],light->location[2]);
                            locationPoint=locationPoint*V;
                            Vector3D planePoint = Vector3D::point((((1/z)*(j-dx))/-d),(((1/z)*(i-dy))/-d),1/z);
                            Vector3D ldp = planePoint-locationPoint;
                            Vector3D lp = -(ldp/ldp.length());
                            double a;
                            double as;
                            if(light->spotAngle==-1){
                                a = Vector3D::dot(normaalvector,lp);
                            } else {
                                as= light->spotAngle*(pi/180);
                                a = 1-(1-Vector3D::dot(normaalvector,lp))/(1-cos(as));
                            }
                            if (a<0){
                                continue;
                            }
                            Vector3D rV = 2*normaalvector*a-lp;
                            Vector3D cameraV = -planePoint;
                            cameraV= cameraV/cameraV.length();
                            rV= rV/rV.length();
                            double b = pow(Vector3D::dot(rV,cameraV),(*it).reflectionCoefficient);
                            if(Vector3D::dot(rV,cameraV)<0){
                                b=0;
                            }
                            if(shadowEnabledParsed){
                                Vector3D lightpoint=planePoint*Vcopy;
                                lightpoint=lightpoint*light->V_shadow;
                                lightpoint.x=(light->dSdxSdy[0]*lightpoint.x)/-lightpoint.z+light->dSdxSdy[1];
                                lightpoint.y=(light->dSdxSdy[0]*lightpoint.y)/-lightpoint.z+light->dSdxSdy[2];
                                double Za=light->shadow_buffer.buffer[floor(lightpoint.x)][floor(lightpoint.y)].first;
                                double Zb=light->shadow_buffer.buffer[floor(lightpoint.x)+1][floor(lightpoint.y)].first;
                                double Zc=light->shadow_buffer.buffer[floor(lightpoint.x)][floor(lightpoint.y)+1].first;
                                double Zd=light->shadow_buffer.buffer[floor(lightpoint.x)+1][floor(lightpoint.y)+1].first;
                                double ax= lightpoint.x-floor(lightpoint.x);
                                double ay= lightpoint.y-floor(lightpoint.y);
                                double Ze= (1-ax)*Za+ax*Zb;
                                double Zf= (1-ax)*Zc+ax*Zd;
                                double zlV = ay*Ze+(1-ay)*Zf;
                                if(abs(1/lightpoint.z-zlV)>0.00001 && 1/lightpoint.z<0 && 1/lightpoint.z>Za && 1/lightpoint.z>Zb && 1/lightpoint.z>Zc && 1/lightpoint.z>Zd){
                                    continue;
                                }
                            }
                            if(!morelights){
                                if(tempcolor.red+round((*it).diffuse[0]*light->diffuseLight[0]*a*255+(*it).specular[0]*light->specularLight[0]*b*255)>=255){
                                    tempcolor.red=255;
                                } else {
                                    tempcolor.red+=round((*it).diffuse[0]*light->diffuseLight[0]*a*255+(*it).specular[0]*light->specularLight[0]*b*255);
                                }
                                if(tempcolor.green+round((*it).diffuse[1]*light->diffuseLight[1]*a*255+(*it).specular[1]*light->specularLight[1]*b*255)>=255){
                                    tempcolor.green=255;
                                } else {
                                    tempcolor.green+=round((*it).diffuse[1]*light->diffuseLight[1]*a*255+(*it).specular[1]*light->specularLight[1]*b*255);
                                }
                                if(tempcolor.blue+round((*it).diffuse[2]*light->diffuseLight[2]*a*255+(*it).specular[2]*light->specularLight[2]*b*255)>=255){
                                    tempcolor.blue=255;
                                } else {
                                    tempcolor.blue+=round((*it).diffuse[2]*light->diffuseLight[2]*a*255+(*it).specular[2]*light->specularLight[2]*b*255);
                                }
                            } else {
                                if(Z.buffer[std::round(j)][std::round(i)].second.red+round((*it).diffuse[0]*light->diffuseLight[0]*a*255+(*it).specular[0]*light->specularLight[0]*b*255)>=255){
                                    tempcolor.red=255;
                                } else {
                                    tempcolor.red=round((*it).diffuse[0]*light->diffuseLight[0]*a*255+Z.buffer[std::round(j)][std::round(i)].second.red+(*it).specular[0]*light->specularLight[0]*b*255);
                                }
                                if(Z.buffer[std::round(j)][std::round(i)].second.green+round((*it).diffuse[1]*light->diffuseLight[1]*a*255+(*it).specular[1]*light->specularLight[1]*b*255)>=255){
                                    tempcolor.green=255;
                                } else {
                                    tempcolor.green=round((*it).diffuse[1]*light->diffuseLight[1]*a*255+Z.buffer[std::round(j)][std::round(i)].second.green+(*it).specular[1]*light->specularLight[1]*b*255);
                                }
                                if(Z.buffer[std::round(j)][std::round(i)].second.blue+round((*it).diffuse[2]*light->diffuseLight[2]*a*255+(*it).specular[2]*light->specularLight[2]*b*255)>=255){
                                    tempcolor.blue=255;
                                } else {
                                    tempcolor.blue=round((*it).diffuse[2]*light->diffuseLight[2]*a*255+Z.buffer[std::round(j)][std::round(i)].second.blue+(*it).specular[2]*light->specularLight[2]*b*255);
                                }
                            }
                            diffuseSucces= true;
                            Z.buffer[std::round(j)][std::round(i)].second=tempcolor;
                            morelights= true;
                        } else if (light->infinity&&specular){
                            img::Color tempcolor=(*it).color;
                            Vector3D planePoint = Vector3D::point((((1/z)*(j-dx))/-d),(((1/z)*(i-dy))/-d),1/z);
                            Vector3D ld = Vector3D::vector(light->direction[0],light->direction[1],light->direction[2]);
                            Vector3D l = -(ld/ld.length());
                            l=l*V;
                            double a = Vector3D::dot(normaalvector,l);
                            if (a<0){
                                continue;
                            }
                            Vector3D rV = 2*normaalvector*a-l;
                            Vector3D cameraV = -planePoint;
                            cameraV= cameraV/cameraV.length();
                            rV= rV/rV.length();
                            double b = pow(Vector3D::dot(rV,cameraV),(*it).reflectionCoefficient);
                            if(Vector3D::dot(rV,cameraV)<0){
                                b=0;
                            }
                            if(!morelights){
                                if(tempcolor.red+round((*it).diffuse[0]*light->diffuseLight[0]*a*255+(*it).specular[0]*light->specularLight[0]*b*255)>=255){
                                    tempcolor.red=255;
                                } else {
                                    tempcolor.red+=round((*it).diffuse[0]*light->diffuseLight[0]*a*255+(*it).specular[0]*light->specularLight[0]*b*255);
                                }
                                if(tempcolor.green+round((*it).diffuse[1]*light->diffuseLight[1]*a*255+(*it).specular[1]*light->specularLight[1]*b*255)>=255){
                                    tempcolor.green=255;
                                } else {
                                    tempcolor.green+=round((*it).diffuse[1]*light->diffuseLight[1]*a*255+(*it).specular[1]*light->specularLight[1]*b*255);
                                }
                                if(tempcolor.blue+round((*it).diffuse[2]*light->diffuseLight[2]*a*255+(*it).specular[2]*light->specularLight[2]*b*255)>=255){
                                    tempcolor.blue=255;
                                } else {
                                    tempcolor.blue+=round((*it).diffuse[2]*light->diffuseLight[2]*a*255+(*it).specular[2]*light->specularLight[2]*b*255);
                                }
                            } else {
                                if(Z.buffer[std::round(j)][std::round(i)].second.red+round((*it).diffuse[0]*light->diffuseLight[0]*a*255+(*it).specular[0]*light->specularLight[0]*b*255)>=255){
                                    tempcolor.red=255;
                                } else {
                                    tempcolor.red=round((*it).diffuse[0]*light->diffuseLight[0]*a*255+Z.buffer[std::round(j)][std::round(i)].second.red+(*it).specular[0]*light->specularLight[0]*b*255);
                                }
                                if(Z.buffer[std::round(j)][std::round(i)].second.green+round((*it).diffuse[1]*light->diffuseLight[1]*a*255+(*it).specular[1]*light->specularLight[1]*b*255)>=255){
                                    tempcolor.green=255;
                                } else {
                                    tempcolor.green=round((*it).diffuse[1]*light->diffuseLight[1]*a*255+Z.buffer[std::round(j)][std::round(i)].second.green+(*it).specular[1]*light->specularLight[1]*b*255);
                                }
                                if(Z.buffer[std::round(j)][std::round(i)].second.blue+round((*it).diffuse[2]*light->diffuseLight[2]*a*255+(*it).specular[2]*light->specularLight[2]*b*255)>=255){
                                    tempcolor.blue=255;
                                } else {
                                    tempcolor.blue=round((*it).diffuse[2]*light->diffuseLight[2]*a*255+Z.buffer[std::round(j)][std::round(i)].second.blue+(*it).specular[2]*light->specularLight[2]*b*255);
                                }
                            }
                            diffuseSucces= true;
                            Z.buffer[std::round(j)][std::round(i)].second=tempcolor;
                            morelights= true;
                        }
                    }
                    if(!diffuseSucces){
                        Z.buffer[std::round(j)][std::round(i)].second=(*it).color;
                    }
                }
            }
        }
    }
    int width=0;
    int height=0;
    for(vector<vector<pair<double,img::Color>>>::const_iterator it=Z.buffer.begin(); it!=Z.buffer.end(); it++,width++){
        height=0;
        for(vector<pair<double,img::Color>>::const_iterator it2=(*it).begin(); it2!=(*it).end(); it2++,height++){
            image(width,height)=(it2->second);
        }
    }
    return image;
}

img::EasyImage generate_image(const ini::Configuration &configuration)
{
    ini::Section general = configuration.operator[]("General");
    ini::Entry type = general.operator[]("type");
    std::string c;
    type.as_string_if_exists(c);
    if (c=="2DLSystem") {
        img::EasyImage image = Lsystem(configuration);
        return image;
    }
    if(c=="Wireframe"){
        img::EasyImage image = ThreeDsystem(configuration);
        return image;
    }
    if(c=="ZBufferedWireframe"){
        img::EasyImage image = ThreeDsystemZBuffered(configuration);
        return image;
    }
    if(c=="LightedZBuffering"||c=="ZBuffering"){
        img::EasyImage image = ThreeDsystemZBufferedLighting(configuration);
        return image;
    }
}

int main(int argc, char const* argv[])
{
    int retVal = 0;
    clock_t start = clock();
    try
    {
        std::vector<std::string> args = std::vector<std::string>(argv+1, argv+argc);
        if (args.empty()) {
            std::ifstream fileIn("filelist");
            std::string filelistName;
            while (std::getline(fileIn, filelistName)) {
                args.push_back(filelistName);
            }
        }
        for(std::string fileName : args)
        {
            ini::Configuration conf;
            try
            {
                std::ifstream fin(fileName);
                if (fin.peek() == std::istream::traits_type::eof()) {
                    std::cout << "Ini file appears empty. Does '" <<
                              fileName << "' exist?" << std::endl;
                    continue;
                }
                fin >> conf;
                fin.close();
            }
            catch(ini::ParseException& ex)
            {
                std::cerr << "Error parsing file: " << fileName << ": " << ex.what() << std::endl;
                retVal = 1;
                continue;
            }

            img::EasyImage image = generate_image(conf);
            if(image.get_height() > 0 && image.get_width() > 0)
            {
                std::string::size_type pos = fileName.rfind('.');
                if(pos == std::string::npos)
                {
                    //filename does not contain a '.' --> append a '.bmp' suffix
                    fileName += ".bmp";
                }
                else
                {
                    fileName = fileName.substr(0,pos) + ".bmp";
                }
                try
                {
                    std::ofstream f_out(fileName.c_str(),std::ios::trunc | std::ios::out | std::ios::binary);
                    f_out << image;
                    f_out.close();

                }
                catch(std::exception& ex)
                {
                    std::cerr << "Failed to write image to file: " << ex.what() << std::endl;
                    retVal = 1;
                }
            }
            else
            {
                std::cout << "Could not generate image for " << fileName << std::endl;
            }
        }
    }
    catch(const std::bad_alloc &exception)
    {
        //When you run out of memory this exception is thrown. When this happens the return value of the program MUST be '100'.
        //Basically this return value tells our automated test scripts to run your engine on a pc with more memory.
        //(Unless of course you are already consuming the maximum allowed amount of memory)
        //If your engine does NOT adhere to this requirement you risk losing points because then our scripts will
        //mark the test as failed while in reality it just needed a bit more memory
        std::cerr << "Error: insufficient memory" << std::endl;
        retVal = 100;
    }
    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %.5f\n", elapsed);
    return retVal;
}
