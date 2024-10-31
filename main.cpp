#include <iostream>
#include<vector>
#include<string>
#include <deque>
using namespace std;

class memory
{
public:
    string st;
    vector<string>vec;
    memory(): vec(256,"00"){}


};

class Register
{
public:
    char c;
    deque<string>vec;
    //Register():vec(256,"00"){}
    void initialize (string s)
    {
        vec.push_front(s);
    }

};


int main()
{
    char v;
    Register one;
    memory two;
    cin>>v>>one.c>>two.st;

    while(true)
    {
        if(v=='C')
        {
            break;
        }
        else
        {
            one.initialize(to_string(v)+ to_string(one.c));
            cin>>v>>one.c>>two.st;
        }
    }
    for(auto i:one.vec)
    {
        cout<<i<<endl;
    }


}
