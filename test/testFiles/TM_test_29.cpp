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