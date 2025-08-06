#include<bits/stdc++.h>
using namespace std;

class Ishape{
public:
    virtual ~Ishape(){}
    virtual void draw() = 0;
};

class LegacyRectangle{
public:
    void draw(int x, int y, int w, int h){
        cout<<"Drawing Legacy Rectangle at (" <<x<<", "<<y<<") of heigth "<<w<<"*"<<h<<endl;
    }
};
                                   
class RectangleAdapter : public Ishape{
    shared_ptr<LegacyRectangle> adaptee;
    int x, y, w, h;
public:
    RectangleAdapter(shared_ptr<LegacyRectangle> legacyRectangle, int x, int y, int w, int h) : adaptee(legacyRectangle), x(x), y(y), w(w), h(h) {}

    void draw(){
        cout<<"Drawing Rectangle"<<endl;
        adaptee->draw(x, y, w, h);
    }
};

int main(){
    
    auto rec = make_shared<RectangleAdapter>(make_shared<LegacyRectangle>(), 0, 0, 2, 3);

    rec->draw();
    return 0;
}