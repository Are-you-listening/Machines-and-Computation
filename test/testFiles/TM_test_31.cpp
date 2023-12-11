img::EasyImage Lsystem(const ini::Configuration &configuration){
    srand(time(NULL));
    ini::Section general = configuration.operator[]("General");
    ini::Entry size = general.operator[]("size");
    ini::Entry backgroundcolor = general.operator[]("backgroundcolor");
    ini::Section _2DLSystem = configuration.operator[]("2DLSystem");
    ini::Entry inputfile = _2DLSystem.operator[]("inputfile");
    ini::Entry color = _2DLSystem.operator[]("color");
    return image;
}