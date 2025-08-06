#include<bits/stdc++.h>
using namespace std;

class Node{
public:
    string name;
    shared_ptr<Node> parent;
    virtual ~Node() = default;
    Node() = default;
    Node(string name, shared_ptr<Node> parent) : name(name), parent(parent) {}

    virtual shared_ptr<Node> getChild(string name){
        throw logic_error("Not Implemented");
    }

    virtual shared_ptr<Node> getChildRegex(string name){
        throw logic_error("Not Implemented");
    }
    
    virtual void addChild(shared_ptr<Node> child){
        throw logic_error("Not Implemented");
    }

    virtual void removeChild(string name){
        throw logic_error("Not Implemented");
    }

    virtual string getContent(string name){
        throw logic_error("Not Implemented");
    }

    virtual void addContent(string name){
        throw logic_error("Not Implemented");
    }
};

class File: public Node{
    string content;

public:
    File() = default;
    File(string name, shared_ptr<Node> parent) : Node(name, parent) {}
    string getContent(){
        return content;
    }

    void addContent(string name){
        throw logic_error("Not Implemented");
    }

};

class Directory: public Node{
    map<string, shared_ptr<Node>> children;
public:
    Directory() = default;
    Directory(string name, shared_ptr<Node> parent) : Node(name, parent) {}

    shared_ptr<Node> getChild(string name){
        if(children.count(name))
            return children[name];

        return nullptr;
    }

    shared_ptr<Node> getChildRegex(string name){
        regex regexp(name);

        for(auto [cname, node]: children){
            if(regex_match(cname, regexp))
                return node;
        }

        return nullptr;
    }
    
    void addChild(shared_ptr<Node> child){
        children[child->name] = child;
    }

    void removeChild(string name){
        if(children.count(name)){
            children.erase(name);
        }
    }


};


class FileSystem{
    shared_ptr<Node> root;

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

    shared_ptr<Node> getNode(string path){
        vector<string> tokens = tokenize(path, '/');
        shared_ptr<Node> cur = root;
        for(auto token: tokens){
            cur = cur->getChild(token);
            if(!cur)
                return cur;
        }

        return cur;
    }

    shared_ptr<Node> getNodeRegex(string path){
        vector<string> tokens = tokenize(path, '/');
        shared_ptr<Node> cur = root;
        for(auto token: tokens){
            cur = cur->getChildRegex(token);
            if(!cur)
                return cur;
        }

        return cur;
    }

    shared_ptr<Node> addParent(string path){
        vector<string> tokens = tokenize(path, '/');
        tokens.pop_back();
        shared_ptr<Node> cur = root;
        for(auto token: tokens){
            auto node = cur->getChild(token);
            if(!node){
                node = make_shared<Directory>(token, cur);
                cur->addChild(node);
            }
            cur = node;
        }

        return cur;
    }

public:
    FileSystem(): root(make_shared<Directory>()) {}
    
    void mkdir(string path){
        vector<string> tokens = tokenize(path, '/');
        shared_ptr<Node> par = addParent(path);
        par->addChild(make_shared<Directory>(tokens.back(), par));
    }

    shared_ptr<Node> cd(string path){
        auto res = getNodeRegex(path);
        if(!res)
            throw logic_error("Not Found");

        return res;
    }

    void pwd(shared_ptr<Node> node){
        auto cur = node;
        vector<string> path;
        while(cur){
            path.push_back(cur->name);
            cur = cur->parent;
        }

        for(auto it = path.rbegin(); it !=path.rend(); ++it){
            cout<<*it<<"/";
        }
        cout<<endl;
    }

};

int main(){

    FileSystem fs;
    fs.mkdir("/home/user/docs");
    auto n1 = fs.cd("/home/user/docs");
    fs.pwd(n1);             // → /home/user/docs

    auto n2 = fs.cd("/home");
    fs.pwd(n2);      

    return 0;
}