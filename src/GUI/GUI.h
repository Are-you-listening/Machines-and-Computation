//
// Created by tibov on 18/12/23.
//

#ifndef TOG_GUI_H
#define TOG_GUI_H

#include "lib/ImGUI/imgui.h"
#include "lib/ImGUI/imgui_impl_glfw.h"
#include "lib/ImGUI/imgui_impl_opengl3.h"
#include <cstdio>
#include <GLFW/glfw3.h>

#include "../TM/TuringMachine.h"
#include "../TM/Creation/TMBuilder.h"

#include <iostream>
#include <thread>
#include <ctime>

#include "src/Tokenisation.h"
#include "src/CFG.h"
#include "src/ThreadFunction.h"
#include "filesystem"
#include "src/Config.h"
#include "src/CFGConstructor.h"
#include "src/LALR.h"
#include "src/GUI/GUI.h"


class GUI {
public:
    GUI();
    ~GUI();
private:
    void windowSetup();
    void setupInput();
    void setupOutput();
    void Config();
    void TMEmulator();

    static const unsigned int max_char = 10000;
    char input_text[max_char] = "";
    string output_text;

    int split_nesting = 1;
    int max_nesting = 3;

    int tuple_size = 4;

    bool lalr = true;
    bool single_tape = false;
    bool if_else_antinesting = false;

    TuringMachine tm_machine;
    TMBuilder* tm_b;

    void threading_check();


};


#endif //TOG_GUI_H
