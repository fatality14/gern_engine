#pragma once

#include <common/common.h>
#include <common/loader.h>
#include <filesystem>
#include <queue>
#include <common/icommand.h>
#include <map>

template<typename T>
class CommandIterator : private ULoader{
public:
    CommandIterator(const filesystem::path& filePath, T& context){
        path = filePath;
        loadLines();
        this->context = &context;
    }

    bool step() {
        if (lines.size() != 0) {
            string& currLine = lines.front();

            ULoader::removeExcessSpaces(currLine);

            command = ULoader::bite(" ", currLine);
            args = currLine;

            lines.pop();

            return true;
        } else {
            return false;
        }
    }

    string getNextCommand() {
        string nextCommand = lines.front();
        return ULoader::bite(" ", nextCommand);
    }

    bool isNextCommand(const string& command) {
        return getNextCommand() == command;
    }

    void execute(const map<string, ICommand<T>&>& commandList) {
        bool found = commandList.count(command);
        if(found){
            commandList.at(command).execute(*context);
        }
        else{
            cout << "Warning: no such command " << command << " found while loading scene - " << path << endl;
        }
    }

    string args;
    string command;
    filesystem::path path;
    queue<string> lines;
    T* context;
private:
    void loadLines(){
        ifstream f;
        f.open(path);

        string line;

        if (f.fail()) {
            throw string("Cannot open file: ") + path.string();
        }

        while (!f.eof()) {
            getline(f, line);
            removeExcessSpaces(line);
            lines.push(line);
        }
    }
};