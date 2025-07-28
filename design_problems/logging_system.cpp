#include<bits/stdc++.h>
using namespace std;

enum class Level{
    DEBUG = 1,
    INFO = 2,
    ERROR = 3
};

map<Level, string> levelString = { 
    {Level::DEBUG, "DEBUG"},
    {Level::INFO, "INFO"},
    {Level::ERROR, "ERROR"}
};

class LogMessage{
public:
    Level level;
    string content;
    int timestamp;
};

class LogAppender{
public:
    virtual void write(string log) = 0; 
};

class ConsoleAppender:public LogAppender{
public:
    void write(string log){
        cout<<log<<endl;
    }
};

class LogFormatter{
public:
    virtual ~LogFormatter() = default;
    virtual string format(LogMessage message) = 0;
};

class StringFormatter:public LogFormatter{
public:
    string format(LogMessage message){
        stringstream ss;
        ss<<message.timestamp<<" "<<levelString[message.level]<<" "<<message.content;
        return ss.str();
    }
};

class LogHandler{
protected:
    shared_ptr<LogHandler> nextHandler;
    Level handlerLevel;
    shared_ptr<LogAppender> appender;
    shared_ptr<LogFormatter> formatter;
public:
    virtual ~LogHandler() = default;

    LogHandler(Level level, shared_ptr<LogAppender> appender, shared_ptr<LogFormatter> formatter): handlerLevel(level), appender(appender), formatter(formatter) {}

    void setNext(shared_ptr<LogHandler> nextHandler){
        this->nextHandler = nextHandler;
    }

    virtual void handle(LogMessage message){
        if(message.level >= handlerLevel){
            appender->write(formatter->format(message));
        }
        else{
            if(nextHandler != nullptr){
            cout<<" Forwarding to next Handler..."<<endl;
            nextHandler->handle(message);
            }
            else{
                cout<<"No handler type available"<<endl;
            }
        }
    }
};

class ErrorLog:public LogHandler{
public:
    ErrorLog(shared_ptr<LogAppender> appender, shared_ptr<LogFormatter> formatter): LogHandler(Level::ERROR, appender, formatter){}
};

class InfoLog:public LogHandler{
public:
    InfoLog(shared_ptr<LogAppender> appender, shared_ptr<LogFormatter> formatter): LogHandler(Level::INFO, appender, formatter){
        setNext(make_shared<ErrorLog>(appender, formatter));
    }
};

class DebugLog:public LogHandler{
public:
    DebugLog(shared_ptr<LogAppender> appender, shared_ptr<LogFormatter> formatter): LogHandler(Level::DEBUG, appender, formatter){
        setNext(make_shared<InfoLog>(appender, formatter));
    }
};


int main(){
    string s = "Testing log";
    auto logger = make_shared<InfoLog>(make_shared<ConsoleAppender>(), make_shared<StringFormatter>());
    LogMessage log{Level::ERROR, s, 12345};
    logger->handle(log);

    return 0;
}