
#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <random>
#include <string>
#include <unordered_map>
#include <cstdlib>
// last changes table
using namespace std;
unordered_map<std::string, std::filesystem::file_time_type> lastChanges;

struct Point {
    double x;
    double y;
};


void onExit() {
    // kill all processes with the name test
    std::string command_pre = "killall final_project";
    system(command_pre.c_str());
    // kill all terminals that are not the current one
    std::string command_post = "osascript -e 'tell application \"Terminal\" to close (every window whose name is not \"Terminal\")'";
    system(command_post.c_str());
}

int listen()
{

    cout << "Listening for changes in /main.cpp" << endl;
    while (true)
    {

        // listen for changes in files in ../../CPP and reload the program using make command
        // get the current path
        string currentPath = filesystem::current_path();

        // get the path of the parent directory
        string parentPath = filesystem::path(currentPath).parent_path();
        parentPath += "/main.cpp";

        // check if there were changes in the parent directory
        filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(parentPath);

        // if there were changes, reload the program
        if (lastChanges[parentPath] != lastWriteTime)
        {
            lastChanges[parentPath] = lastWriteTime;
            std::cout << "reloading program" << std::endl;

            // kill all processes with the name test
            std::string command_pre = "killall final_project";
            system(command_pre.c_str());
            // kill all terminals that are not the current one
            std::string command_post = "osascript -e 'tell application \"Terminal\" to close (every window whose name is not \"Terminal\")'";
            system(command_post.c_str());
            // execute make command on ../../CPP
            std::system("make -C ../");
            // generate random 4 points for lines as arguments
            std::string directory = "/Users/nadavavnon/Desktop/GL-Games/checkers-3d";
            std::string program = "./final_project"; // Replace this with the name of the program you want to execute

            std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<int> dist(10, 630);

            string args = "";
            args = std::to_string(dist(rng)) + " " + std::to_string(dist(rng)) + " " + std::to_string(dist(rng)) + " " + std::to_string(dist(rng)) + " " + std::to_string(dist(rng)) + " " + std::to_string(dist(rng)) + " " + std::to_string(dist(rng)) + " " + std::to_string(dist(rng));
            std::string command = "osascript -e 'tell application \"Terminal\" to do script \"cd " + directory + " && " + program + " " + args + "\"'";
            std::system(command.c_str());
        }

        // sleep for 10 second
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main()
{
    std::atexit(onExit);
    // 
    // spawn listen in a separate thread
    std::thread listenThread(listen);

    // wait for the thread to finish
    listenThread.join();

    return 0;
}