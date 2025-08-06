#include<bits/stdc++.h>
using namespace std;

class FileSystemNode{
public:
    string name;
    virtual ~FileSystemNode() {}
    FileSystemNode() = default;
    FileSystemNode(string name): name(name) {}

    virtual shared_ptr<FileSystemNode> getChild(string name){
        throw logic_error("Not implemented");
    }

    virtual vector<string> getChildrenNames(){
        throw logic_error("Not implemented");
    }

    virtual void addChild(shared_ptr<FileSystemNode> child){
        throw logic_error("Not implemented");
    }

    virtual void removeChild(string child){
        throw logic_error("Not implemented");
    }

    virtual string getContent(){
        throw logic_error("Not implemented");
    }

    virtual string addContent(string content){
        throw logic_error("Not implemented");
    }  

    virtual bool empty(){
        throw logic_error("Not implemented");
    }
};

class File:public FileSystemNode{
    string content;
    string extension;
public:
    File(string name):FileSystemNode(name) {}

    string getContent(){
        return content;
    }

    string addContent(string cont){
        content += cont;
        return content;
    }  

    bool empty(){
        return content.empty();
    }
    
};

class Directory:public FileSystemNode{
    map<string, shared_ptr<FileSystemNode>> children;
public:
    Directory() = default;
    Directory(string name):FileSystemNode(name) {}

    shared_ptr<FileSystemNode> getChild(string name){
        if(children.count(name))
            return children[name];
        
        return nullptr;
    }

    vector<string> getChildrenNames(){
        vector<string> res;
        for(auto [name, child]:children)
            res.push_back(name);

        return res;
    }

    void addChild(shared_ptr<FileSystemNode> child){
        children[child->name] = child;
    }   

    void removeChild(string child){
        children.erase(child);
    }

    bool empty(){
        return children.empty();
    }

};

class FileSystem{
    shared_ptr<FileSystemNode> root;

    vector<string> tokenize(string path, char delim){
        stringstream ss(path);
        string pathElement;
        vector<string> tokens;

        while(getline(ss, pathElement, delim)){
            if(!pathElement.empty())
                tokens.push_back(pathElement);
        }

        return tokens;
    }

    shared_ptr<FileSystemNode> getNode(string path){
        vector<string> tokens = tokenize(path, '/');

        auto cur = root;
        for (auto &token : tokens) {
            cur = cur->getChild(token);
            if(!cur)
                return cur;
        }
        return cur;
    }

    shared_ptr<FileSystemNode> addParent(string path){
        vector<string> tokens = tokenize(path, '/');
        tokens.pop_back();

        auto cur = root;
        for (auto &token : tokens) {
            auto next = cur->getChild(token);
            if(!next){
                next = make_shared<Directory>(token);
                cur->addChild(next);
            }
            cur = next;
        }
        return cur;
    }

    void dfs(int ind, shared_ptr<FileSystemNode> par, vector<string> &tokens){

        if(par == nullptr)
            return;

        if(ind == tokens.size()-1){
            par->removeChild(tokens[ind]);
            return;
        }

        dfs(ind+1, par->getChild(tokens[ind]), tokens);
        if(par->getChild(tokens[ind])->empty())
            par->removeChild(tokens[ind]);        
    }

    template<typename K, typename V>
    bool isType(V& v){
        return typeid(K)==typeid(v);
    }
    

public:

    FileSystem(): root(make_shared<Directory>()) {}

    void mkdir(string path){
        vector<string> tokens = tokenize(path, '/');
        shared_ptr<FileSystemNode> par = addParent(path);
        par->addChild(make_shared<Directory>(tokens.back()));
    }

    void addContent(string path, string content){
        shared_ptr<FileSystemNode> file = getNode(path);
        if(!file){
            vector<string> tokens = tokenize(path, '/');
            file = make_shared<File>(tokens.back());
            shared_ptr<FileSystemNode> par = addParent(path);
            par->addChild(file);
        }

        file->addContent(content);
    }

    void ls(string path){
        auto cur = getNode(path);

        if(cur == nullptr)
            return;

        if(isType<File>(*cur)){
            cout<<cur->name<<endl;
        }
        else{
            auto names = cur->getChildrenNames();
            for(auto &name:names)
                cout<<name<<" ";
            cout<<endl;
        }
    }

    string readContent(string path){
        auto file = getNode(path);

        return file->getContent();
    }

    void rm(string path){
        vector<string> tokens = tokenize(path, '/');
        return dfs(0, root, tokens);
    }

};

int main() {
    FileSystem fs;

    cout << "---- mkdir & ls ----\n";
    fs.mkdir("/docs");
    fs.mkdir("/docs/projects");
    fs.ls("/");            // expected: docs
    fs.ls("/docs");        // expected: projects

    cout << "\n---- write & append ----\n";
    fs.addContent("/docs/projects/readme.txt", "Hello");
    fs.addContent("/docs/projects/readme.txt", " World!");
    fs.ls("/docs/projects");   // expected: readme.txt
    cout << "Content: " << fs.readContent("/docs/projects/readme.txt") << '\n';

    cout << "\n---- nested mkdir & files ----\n";
    fs.mkdir("/music/rock");
    fs.addContent("/music/rock/song.lyrics", "We will, we will rock you");
    fs.ls("/music");       // expected: rock
    fs.ls("/music/rock");  // expected: song.lyrics

    cout << "\n---- testing rm (recursive clean-up) ----\n";
    fs.rm("/docs/projects/readme.txt");     // remove file
    fs.ls("/docs/projects");                // expected: (empty line)
    fs.rm("/docs");                         // remove dir; should vanish because empty
    fs.ls("/");                             // expected: music

    return 0;
}
