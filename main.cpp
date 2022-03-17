#include <ncurses.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <cstring>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <vector>
#include <cstdio>
#include <cstdlib>


using namespace std;

std::string exec(const char* cmd, std::vector<std::string> *history);

int main(){
    int ch;
    bool ended = false;

    std::vector <std::string> history;
    char cmd[100] = "";
    std::string str, history_string;

    char tmp[256];
    getcwd(tmp, 256);
    int c, ex;
    int cnt = 0;
    bool historied = false;

    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();

    while(true)
    {
        getcwd(tmp, 256);
        if(!historied)
        {
            printw(tmp);
            printw("$ ");
        }

        memset(cmd, 0, 100);
        ended = false;
        while ((ch = getch()) != '#' && !ended) {

            switch(ch) {
                case KEY_UP: {
                    cnt++;
                    if ((int) history.size() - (int) cnt >= 0) {
                        int len = strlen(cmd);
                        while (len) {
                            addstr("\b \b");
                            len--;
                        }
                        if (historied) {
                            len = history_string.size();
                            while (len) {
                                addstr("\b \b");
                                len--;
                            }
                        }
                        strcpy(cmd, history[history.size() - cnt].c_str());
                        printw(cmd);
                        historied = true;


                    } else
                        cnt--;
                    break;
                }
                case KEY_DOWN: if(historied && cnt > 1)
                    {

                        cnt--;
                        if ((int) history.size() - (int) cnt >= 0) {
                            int len = strlen(cmd);
                            while (len) {
                                addstr("\b \b");
                                len--;
                            }
                            if (historied) {
                                len = history_string.size();
                                while (len) {
                                    addstr("\b \b");
                                    len--;
                                }
                            }
                            strcpy(cmd, history[history.size() - cnt].c_str());
                            printw(cmd);
                            historied = true;


                        } else
                            cnt++;
                    }
                    break;

                case KEY_BACKSPACE:
                    addstr("\b \b");
                    cmd[strlen(cmd) - 1] = '\0';
                    break;
                case KEY_ENTER:
                    ended = true;
                    historied = false;
                    cnt = 0;
                    if (strcmp(cmd, "exit") == 0)
                        exit(0);

                    if(cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') //Special case change directory
                    {
                        chdir(cmd + 3);
                        history.emplace_back(cmd);
                        printw("\n");
                    }
                    else
                    {
                        string string1 = exec(cmd, &history);
                        strcpy(cmd, string1.c_str());
                        printw("\n");
                        addstr(cmd);
                    }


                    break;

                default:
                    printw("%c", ch);


                    cmd[strlen(cmd) + 1] = '\0';
                    cmd[strlen(cmd)] = (char)ch;


            }
        }
        refresh();
        //getch();
        endwin();
    }

}

std::string exec(const char* cmd, std::vector<std::string> *history) {
    history->emplace_back(cmd);
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}