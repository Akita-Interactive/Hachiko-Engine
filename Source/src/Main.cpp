#include "core/hepch.h"
#include "instrumentation/MemoryLeaks.h"

enum class MainStates
{
    MAIN_CREATION,
    MAIN_START,
    MAIN_UPDATE,
    MAIN_FINISH,
    MAIN_EXIT
};

Hachiko::Application* App = nullptr;

int main(int argc, char** argv)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    int main_return = EXIT_FAILURE;
    auto state = MainStates::MAIN_CREATION;

    while (state != MainStates::MAIN_EXIT)
    {
        switch (state)
        {
        case MainStates::MAIN_CREATION: HE_LOG("Application Creation --------------");
            App = new Hachiko::Application();
            state = MainStates::MAIN_START;
            break;

        case MainStates::MAIN_START: HE_LOG("Application Init --------------");
            if (App->Init() == false)
            {
                HE_LOG("Application Init exits with error -----");
                state = MainStates::MAIN_EXIT;
            }
            else
            {
                state = MainStates::MAIN_UPDATE;
                HE_LOG("Application Update --------------");
            }

            break;

        case MainStates::MAIN_UPDATE:
        {
            OPTICK_FRAME("MainThread");
            
            const UpdateStatus update_return = App->Update();

            if (update_return == UpdateStatus::UPDATE_ERROR)
            {
                HE_LOG("Application Update exits with error -----");
                state = MainStates::MAIN_EXIT;
            }

            if (update_return == UpdateStatus::UPDATE_STOP)
                state = MainStates::MAIN_FINISH;
        }
        break;

        case MainStates::MAIN_FINISH: HE_LOG("Application CleanUp --------------");
            if (App->CleanUp() == false)
            {
                HE_LOG("Application CleanUp exits with error -----");
            }
            else
                main_return = EXIT_SUCCESS;

            state = MainStates::MAIN_EXIT;

            break;
        }
    }

    delete App;
    HE_LOG("Bye :)\n");
    return main_return;
}
