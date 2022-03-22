#include<bits/stdc++.h>
using namespace std;

class comp
{
    public:
    int operator() (const pair<int,float> &a,const pair<int,float> &b)
    {
        if(b.second != a.second)
            return b.second < a.second;
        else
            return b.first > a.first;
    }
};

int main()
{
    priority_queue<pair<float,int>, vector<pair<float,int>>,comp> candidates;
    candidates.push({1,3});
    candidates.push({3,1});
    candidates.push({2,2});
    cout << candidates.top().first;

}