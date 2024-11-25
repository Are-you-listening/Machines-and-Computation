//
// Created by tibov on 18/12/23.
//

#include "GUI.h"

static unsigned int core_amount = std::thread::hardware_concurrency();

GUI::GUI() {

    tm_b = new TMBuilder(4, true, 2, 4);
    TMBuilder_output data = tm_b->generateTM();
    tm_machine.load(data.states, data.start_state, data.input, data.tape_size, data.productions);

    auto config = Config::getConfig();
    split_nesting = config->getSplitNesting();
    max_nesting = config->getMaxNesting();
    if_else_antinesting = config->getIfElseNesting();
    threading = config->isThreading();

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

    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImColor(25.0/255.0, 25.0/255.0, 40.0/255.0, 1.00f);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImColor(35.0/255.0, 35.0/255.0, 60.0/255.0, 1.00f);

    ImVec4 clear_color = ImVec4(50.0/255.0, 50.0/255.0, 80.0/255.0, 1.00f);


    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        setupInput();
        setupOutput();
        Config();
        if (tm_busy){
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
    if (tm_busy){
        ImGui::TextColored(output_color, "%s", "TM still in progress");
    }else{
        ImGui::TextColored(output_color, "%s", output_text.c_str());
    }

    ImGui::End();

}

void GUI::Config() {
    auto size = ImGui::GetIO().DisplaySize;
    size.x = size.x/4.0;
    size.y = size.y/4.0;

    ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetIO().DisplaySize.y/2.0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("Config File");


    ImGui::SetNextWindowSize(size);
    bool changed = false;
    if (ImGui::SliderInt("split nesting", &split_nesting, 2, 9)){

        if (split_nesting >= max_nesting){
            max_nesting = split_nesting + 1;
        }

        changed = true;
    }
    if (ImGui::SliderInt("max nesting", &max_nesting, 3, 10)){

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
    }else{
        ImGui::Checkbox("Threading", &threading);

    }

    if (ImGui::Button("Convert")){
        if (input_text[0] == '\u0000'){

        }else{
            if (lalr){
                auto config = Config::getConfig();

                config->setMaxNesting(max_nesting);
                config->setSplitNesting(split_nesting);
                config->setThreading(threading);
                config->setIfElseNesting(if_else_antinesting);


                ofstream file{"input/SandBox/A.cpp"};
                for (char c: input_text){

                    if (c == '\u0000'){
                        break;
                    }
                    file << c;
                }
                file.close();

                Tokenisation tokenVector;
                std::string Filelocation="input/SandBox/A.cpp";
                std::thread Tokenizer(&Tokenisation::Tokenize, &tokenVector, Filelocation);
                core_amount--;

                Orchestrator("");
                auto cfg = createCFG();
                cfg->toGNF();
                const CFG a = *cfg;
                LALR lalry(a);
                lalry.createTable();

                Tokenizer.join();
                core_amount++;

                //create LARL parser with tokenvector
                auto vec = tokenVector.getTokenVector();

                bool check_valid = false;
                if (check_valid){
                    auto cfg_cyk = createCFG();
                    cfg_cyk->toCNF();

                    string check_str;
                    for (auto& v: vec){
                        check_str += std::get<0>(v);
                    }
                    bool b = cfg_cyk->accepts(check_str);
                    cout << "valid file " << b << endl;
                }


                lalry.parse(vec);
                lalry.generate();

                string out = lalry.getYield();

                output_text = out;
                fixTabs();

                if (threading){
                    threading_check();
                    ifstream readThread{"output/result.cppresult.cc"};
                    string text;
                    while (!readThread.eof()){
                        char c = (char) readThread.get();
                        if (c == '\377'){
                            break;
                        }
                        text += c;
                    }

                    output_text = text;
                }
                save();
            }else{

                tm_busy = true;
                move_counter = 0;
                string text_string;
                for (char c: input_text){
                    text_string += c;
                }

                auto data = tm_b->generateTM();

                if (single_tape){
                    tm_machine.clear(true);
                    tm_machine.load(data.states, data.start_state, data.input, data.tape_size, data.productions);
                    tm_machine.setSingleTape(false);
                    tm_machine.load_input(text_string, 1);
                    if (!tm_machine.isSingleTape()){
                        tm_machine = *tm_machine.toSingleTape();
                    }
                }

                if (!single_tape){
                    tm_machine = TuringMachine{};
                    tm_machine.clear(true);
                    tm_machine.load(data.states, data.start_state, data.input, data.tape_size, data.productions);
                    tm_machine.load_input(text_string, 1);
                }

            }
        }



    }



    if (tm_busy){
        int moves_doing = 0;
        if (ImGui::Button("1X")){
            moves_doing = 1;

        }
        ImGui::SameLine();
        if (ImGui::Button("10X")){
            moves_doing = 10;

        }
        ImGui::SameLine();
        if (ImGui::Button("100X")){
            moves_doing = 100;

        }
        ImGui::SameLine();
        if (ImGui::Button("1000X")){
            moves_doing = 1000;

        }

        ImGui::SameLine();
        if (ImGui::Button("10000X")){
            moves_doing = 10000;

        }

        ImGui::SameLine();
        if (ImGui::Button("100000X")){
            moves_doing = 100000;

        }

        for (int m=0; m<moves_doing; m++){
            if (tm_machine.isHalted()){
                break;
            }

            move_counter += 1;
            tm_machine.move();
        }

        if (tm_machine.isHalted()){
            tm_busy = false;
            int index = 1;
            if (tm_machine.isSingleTape()){
                index = 3;
            }
            output_text = tm_machine.exportTapeData(index);
            fixTabs();
            save();
        }
    }

    lalr = !tm;

    if (changed){
        delete tm_b;
        tm_b = new TMBuilder(tuple_size, if_else_antinesting, split_nesting, max_nesting);
    }

    if (ImGui::Button("load F1 (class)")){
        loadDemo("demo/class.cpp");
    }
    if (ImGui::Button("load F2 (combination)")){
        loadDemo("demo/combination.cpp");
    }
    if (ImGui::Button("load F3 (forif)")){
        loadDemo("demo/forif.cpp");
    }
    if (ImGui::Button("load F4 (main)")){
        loadDemo("demo/main.cpp");
    }
    if (ImGui::Button("load F5 (variables)")){
        loadDemo("demo/variables.cpp");
    }

    if (ImGui::Button("load F6 (knapsack)")){
        loadDemo("demo/knapsack.cpp");
    }

    if (tm){
        if (ImGui::Button("load F7 (singletape)")){
            loadDemo("demo/singletape.cpp");
        }

        if (ImGui::Button("load F8 (ifElseAntinesting)")){
            loadDemo("demo/ifElseAntinestingTM.cpp");
        }
    }


    ImGui::End();


}

void GUI::TMEmulator() {
    auto size = ImGui::GetIO().DisplaySize;
    size.x = size.x/4.0;
    size.y = size.y/4.0;


    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x/4.0, ImGui::GetIO().DisplaySize.y/2.0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(size.x*2.5, size.y*2), ImGuiCond_Once);
    ImGui::Begin("TM drawing");

    size = ImGui::GetWindowSize();
    size.x = size.x/4.0;
    size.y = size.y/4.0;

    auto widget_pos = ImGui::GetWindowPos();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();


    int blocks_per_tape = 40;
    int tapes_amount = tm_machine.getTapeAmount();

    float default_offset_x = 0.075*size.x*4;
    float default_offset_y = 0.1*size.y*4;

    string moves_string = "Move: ";
    moves_string += to_string(move_counter);
    moves_string += " State: ";
    moves_string += tm_machine.getCurrentState();
    moves_string += '\u0000';

    draw_list->AddText(ImVec2{widget_pos.x+ (int)(default_offset_x*1.1), widget_pos.y+(int)(default_offset_y*0.5)},ImColor(1.0f, 1.0f, 1.00f, 1.00f), moves_string.c_str());


    float size_y = size.y/5.0*9.0/(double)tapes_amount;
    float size_x = size.y/5.0*9.0/(double)tapes_amount;

    for (int j= 0; j<tapes_amount; j++){


        float offset_y = j*0.07*size.y*4*(9.0/(float)tapes_amount)+ default_offset_y;

        string tape_data = tm_machine.exportTapeData(j, true);
        int i_x = 0;
        for (int i= tm_machine.getTuringIndex(j)-10; i<blocks_per_tape+tm_machine.getTuringIndex(j)-10; i++, i_x++){

            float offset_x = i_x*size_x+ default_offset_x;


            auto output_color = ImColor(1.0f, 1.0f, 1.00f, 1.00f);
            if (i == tm_machine.getTuringIndex(j)){
                output_color = ImColor(0.0f, 1.00f, 0.00f, 1.00f);
            }
            ImGui::SetWindowFontScale(2);
            if (i < 0|| i >= tape_data.size() || tape_data[i] == '\u0000'){
                output_color = ImColor(1.0f, 0.00f, 0.00f, 0.40f);
                if (i == tm_machine.getTuringIndex(j)){
                    output_color = ImColor(1.0f, 0.70f, 0.00f, 0.40f);
                }
                draw_list->AddText(ImVec2{widget_pos.x+offset_x+size_x/8, widget_pos.y+offset_y+size_y/8}, output_color, "B");


            }else{
                string temp;
                temp += tape_data[i];
                draw_list->AddText(ImVec2{widget_pos.x+offset_x+size_x/8, widget_pos.y+offset_y+size_y/8}, output_color, temp.c_str());
            }

            draw_list->AddRect(ImVec2{widget_pos.x+offset_x, widget_pos.y+offset_y}, ImVec2{widget_pos.x+size_x+offset_x, widget_pos.y+size_y+offset_y}, IM_COL32(255, 255, 255, 255));
        }
    }

    ImGui::End();
}

GUI::~GUI() {
    delete tm_b;
}

void GUI::threading_check() {
    Orchestrator::tabber(); //Cleanup output file; match tabs & spaces
    if(Config::getConfig()->isThreading()){
        ThreadFunction::threadFILE("output/result.cpp");
    }


}

void GUI::fixTabs() {
    bool start = true;

    string output;
    for (char c: output_text){
        if (' ' == c && start){
            continue;
        }

        if ('\n' == c){
            start = true;

        }else{
            start = false;
        }

        output.push_back(c);
    }

    output_text = output;
    output.clear();
    output = "";
    start = true;
    string tab = "";
    for(char c: output_text){
        bool add = c == '{';
        bool remove = c == '}';

        if (add){
            tab+="    ";
        }

        if (remove){
            if (tab.size() > 4){
                for (int j =0; j<4; j++){
                    tab.pop_back();
                }
            }else{
                tab.clear();
            }

        }

        if (start){
            if (tab.size() != 0){
                output += tab;
            }

        }

        if ('\n' == c){
            start = true;

        }else{
            start = false;
        }

        output.push_back(c);
    }
    output_text = output;
}

void GUI::save() {
    ofstream before{"GUI_before.cpp"};
    before << input_text;
    before.close();

    ofstream after{"GUI_after.cpp"};
    after << output_text;
    after.close();

}

void GUI::loadDemo(const string &name) {

    ifstream o{name};

    for (int i = 0; i<max_char; i++){
        input_text[i] = '\u0000';
    }

    int counter = 0;
    while (!o.eof() && o.good()){
        char c = (char) o.get();
        if (c == '\377'){
            break;
        }
        input_text[counter] = c;
        counter++;
    }

}
