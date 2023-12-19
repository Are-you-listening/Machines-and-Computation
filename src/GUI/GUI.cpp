//
// Created by tibov on 18/12/23.
//

#include "GUI.h"


GUI::GUI() {

    tm_b = new TMBuilder(4, true, 2, 4);
    TMBuilder_output data = tm_b->generateTM();
    tm_machine.load(data.states, data.start_state, data.input, data.tape_size, data.productions);

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // Create window with graphics context
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280*2, 720*2, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr){
        throw 0;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        setupInput();
        setupOutput();
        Config();
        if (!lalr){
            TMEmulator();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

}

void GUI::setupInput() {
    auto size = ImGui::GetIO().DisplaySize;
    size.x = size.x/2.0;
    size.y = size.y/2.0;

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
    ImGui::Begin("Enter Input Code");
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    ImGui::InputTextMultiline("##source", input_text, IM_ARRAYSIZE(input_text), ImVec2(-FLT_MIN, -FLT_MIN), flags);
    ImGui::End();
    auto v = ImVec2(-FLT_MIN, -FLT_MIN);

}

void GUI::windowSetup() {

}

void GUI::setupOutput() {
    auto size = ImGui::GetIO().DisplaySize;
    size.x = size.x/2.0;
    size.y = size.y/2.0;

    ImGui::SetNextWindowPos(ImVec2(size.x, 0.0f));
    ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
    ImGui::Begin("Output Code");

    ImVec4 output_color = ImVec4(1.0f, 1.0f, 0.00f, 1.00f);
    ImGui::TextColored(output_color, "%s", output_text.c_str());
    ImGui::End();

}

void GUI::Config() {
    auto size = ImGui::GetIO().DisplaySize;
    size.x = size.x/4.0;
    size.y = size.y/4.0;

    ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetIO().DisplaySize.y/2.0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("Config File");

    ImVec4 output_color = ImVec4(1.0f, 1.0f, 0.00f, 1.00f);

    ImGui::SetNextWindowSize(size);
    bool changed = false;
    if (ImGui::SliderInt("split nesting", &split_nesting, 1, 9)){

        if (split_nesting >= max_nesting){
            max_nesting = split_nesting + 1;
        }

        changed = true;
    }
    if (ImGui::SliderInt("max nesting", &max_nesting, 2, 10)){

        if (split_nesting >= max_nesting){
            split_nesting = max_nesting - 1;
        }

        changed = true;
    }

    ImGui::Checkbox("LALR", &lalr);


    bool tm = !lalr;
    if (ImGui::Checkbox("TM", &tm)){
        changed = true;
    }

    if (ImGui::Button("Convert")){
        if (lalr){
            ofstream file{"input/SandBox/A.cpp"};
            for (char c: input_text){
                file << c;
                if (c == '\u0001'){
                    break;
                }
            }
            file.close();

            Tokenisation tokenVector;
            std::string Filelocation="input/SandBox/A.cpp";
            std::thread Tokenizer(&Tokenisation::Tokenize, &tokenVector, Filelocation);

            Orchestrator();
            auto cfg = createCFG();
            cfg->toGNF();

            const CFG a = *cfg;
            LALR lalr(a);
            lalr.createTable();

            Tokenizer.join();

            //create LARL parser with tokenvector
            auto vec = tokenVector.getTokenVector();

            lalr.parse(vec);
            lalr.generate();

            string out = lalr.getYield();
            output_text = out;

        }else{

        }
    }

    if (tm){
        if (ImGui::Checkbox("Singletape", &single_tape)){
            changed = true;
        }
        if (ImGui::Checkbox("IfElse Antinesting", &if_else_antinesting)){
            changed = true;
        }

        if (ImGui::SliderInt("tuple size", &tuple_size, 2, 8)){
            changed = true;
        }
    }

    lalr = !tm;

    if (changed){
        delete tm_b;
        tm_b = new TMBuilder(4, if_else_antinesting, split_nesting, max_nesting);
    }


    ImGui::TextColored(output_color, "%s", input_text);
    ImGui::End();


}

void GUI::TMEmulator() {
    auto size = ImGui::GetIO().DisplaySize;
    size.x = size.x/4.0;
    size.y = size.y/4.0;


    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x/4.0, ImGui::GetIO().DisplaySize.y/2.0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(size.x*2.5, size.y*1.5), ImGuiCond_Once);
    ImGui::Begin("TM drawing");

    size = ImGui::GetWindowSize();
    size.x = size.x/4.0;
    size.y = size.y/4.0;

    auto widget_pos = ImGui::GetWindowPos();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    double i = 0.1;
    double j = 0.1;

    float offset_x = i*size.x*4;
    float offset_y = j*size.y*4;

    float size_x = size.y/2.0;
    float size_y = size.y/2.0;

    draw_list->AddRect(ImVec2{widget_pos.x+offset_x, widget_pos.y+offset_y}, ImVec2{widget_pos.x+size_x+offset_x, widget_pos.y+size_y+offset_y}, IM_COL32(255, 255, 255, 255));
    ImGui::End();
}

GUI::~GUI() {
    delete tm_b;
}

void GUI::threading_check() {
    //threading every function for now, will later be changed
    // I also assume that every function we create to replace nesting is only called upon once
    // result don't work for now, will be changed
    // Function calls in Function calls don't work for now, another function that split those calls up is needed
    //std::string ResultFileLocation="../test/results/TM_handmatig_result.cpp";
    std::string ResultFileLocation="input/SandBox/A.cpp";
    std::string line;
    std::string line2;
    std::ifstream File(ResultFileLocation);
    std::vector<std::string> FunctionCalls;
    while(getline(File,line)){
        if(line[0]!='#'&&line[0]!=' '&&line.substr(0,6)!="static"&&line.substr(0,6)!="struct"&&!line.empty()&&line!="}"&&line!="{"&&line.substr(0,7)!="typedef"&&line.substr(0,8)!="namespace"&&line.substr(0,6)=="void A"&&line.substr(0,2)!="//"){ // I assume the code we check people don't write variables or classes above a function, also needs debugging
            FunctionCalls.push_back(line);
        }
    }
    std::vector<std::thread> Threads; // still doesn't work, remember void functions and their returns, etc.
    ThreadFunction threading; // maybe create a function that turns every function into a void one.
    unsigned long int count=0; // I also assume calling join() on a thread that's already joined is not harmful.
    for(const auto & i : FunctionCalls){
        std::filesystem::copy(ResultFileLocation,ResultFileLocation + std::to_string(count));
        std::thread temp(&ThreadFunction::ThreadFunctionCall, &threading, ResultFileLocation + std::to_string(count), i);
        Threads.push_back(std::move(temp));
    }
    for(std::vector<std::thread>::iterator it=Threads.begin(); it!=Threads.end(); it++){
        it->join();
    }
    File.close();

    std::ofstream File2(ResultFileLocation+"result.cc");
    std::ifstream File1(ResultFileLocation);
    for(unsigned long int i=0; i<count; i++){
        std::ifstream File4(ResultFileLocation + std::to_string(i));
        std::string line4;
        while(getline(File1,line)){
            getline(File4,line4);
            if(line4.find("std::thread a")!=std::string::npos){
                File2 << line4 << std::endl;
            } else {
                File2 << line << std::endl;
            }
            while(line4.find("a000")!=std::string::npos&&line4.find("thread ")==std::string::npos){
                getline(File4,line4);
            }
        }
        std::ifstream File7(ResultFileLocation+"result.cc");
        std::string line7;
        std::ofstream File8(ResultFileLocation+"tempresult.cc0");
        while(getline(File7,line7)){
            File8 << line7 << std::endl;
        }
        File7.close();
        File8.close();
        File2=std::ofstream(ResultFileLocation+"result.cc");
        File1=std::ifstream(ResultFileLocation+"tempresult.cc0");
    }
    File2.close();
    File1.close();

    std::ofstream File5(ResultFileLocation+"result.cc");
    std::ifstream File6(ResultFileLocation+"tempresult.cc0");
    for(unsigned long int i=0; i<count; i++){
        std::ifstream File3(ResultFileLocation + std::to_string(i));
        std::string line3;
        while(getline(File6,line)){
            getline(File3,line3);
            while(line.find("a000")!=std::string::npos&&line.find("thread ")==std::string::npos){
                File5 << line << std::endl;
                getline(File6,line);
            }
            while(line3.find("a000")!=std::string::npos&&line3.find("thread ")==std::string::npos){
                File5 << line3 << std::endl;
                getline(File3,line3);
            }
            File5 << line << std::endl;
        }
        std::ifstream File7(ResultFileLocation+"result.cc");
        std::string line7;
        std::ofstream File8(ResultFileLocation+"tempresult.cc0");
        while(getline(File7,line7)){
            File8 << line7 << std::endl;
        }
        File7.close();
        File8.close();
        File5=std::ofstream(ResultFileLocation+"result.cc");
        File6=std::ifstream(ResultFileLocation+"tempresult.cc0");
    }
    std::ofstream File7(ResultFileLocation+"result.cc");
    std::string line8;
    std::ifstream File8(ResultFileLocation+"tempresult.cc0");
    while(getline(File8,line8)){
        File7 << line8 << std::endl;
    }
    File5.close();
    File6.close();
    File7.close();
    File8.close();

    for(unsigned long int i=0; i<count; i++){
        std::string c=ResultFileLocation + std::to_string(i);
        std::remove(c.c_str());
    }
    std::string c=ResultFileLocation +"tempresult.cc0";
    std::remove(c.c_str());

    std::ifstream File9(ResultFileLocation+"result.cc");
    std::vector<std::string> V;
    std::string C;
    while(getline(File9,C)){
        V.push_back(C);
    }

    std::ofstream File10(ResultFileLocation+"result.cc");
    File10<<"#include <thread>"<<std::endl;
    for(const auto& it:V){
        File10 << it <<std::endl;
    }


}

