#include "EditorApp.hpp"

// Punto de entrada del editor UNIVERSAL ENGINE.
int main(int argc, char* argv[]) {
    EditorApp app;
    if (!app.initialize(argc, argv)) {
        return -1;
    }
    app.run();
    app.shutdown();
    return 0;
}