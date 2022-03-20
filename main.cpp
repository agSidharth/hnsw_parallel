#include <bits/stdc++.h>
using namespace std;

float cosine_dist(vector<float>& x,vector<float>& y)          // can be parallelized
{
    float modX,modY,dotXY;
    for(int idx=0;idx<x.size();idx++)
    {
        modX += x[idx]*x[idx];
        modY += y[idx]*y[idx];
        dotXY += x[idx]*y[idx];
    }

    if(modX == 0 || modY == 0) return 0;
    return float(dotXY)/float(sqrt(modX)*sqrt(modY));
}

void SearchLayer(vector<float>& q,priority_queue<pair<float,int>>& topk,vector<int>& indptr,vector<int>& index,vector<int>& level_offset,int lc,vector<int>& visited,vector<vector<float>>& vect,int k)
{
    priority_queue<pair<float,int>> candidates = topk;          //confirm if it is a priority queue or not..

    while(candidates.size()>0)
    {
        int this_ep = candidates.top().second;
        candidates.pop();

        int start = indptr[this_ep] + level_offset[lc];
        int end = indptr[this_ep] + level_offset[lc+1];
        
        for(int idx=start;idx<end;idx++)                        // can be parallelized..
        {
            int px = index[idx];
            if(px==-1 || visited[px]==1) continue;
            
            visited[px] = 1;
            float _dist = cosine_dist(q,vect[px]);

            if (_dist >= topk.top().first && topk.size()==k) continue;

            if(topk.size()==k) topk.pop();
            topk.push({_dist,px});
            candidates.push({_dist,px});
        }
    }
}


void QueryHNSW(vector<float>& q,vector<int>& thistopk,int ep,vector<int>& indptr,vector<int>& index,vector<int>& level_offset,int max_level,vector<vector<float>>& vect)
{

    priority_queue<pair<float,int>> pq_topk;          //store (distance,node_id)
    pq_topk.push({cosine_dist(q,vect[ep]),ep});   
    vector<int> visited(vect.size(),0);

    visited[ep] = 1;
    for(int lev=max_level;lev>=0;lev--)               // no parallelization possible..
    {
        SearchLayer(q,pq_topk,indptr,index,level_offset,lev,visited,vect,thistopk.size());
    }

    int total_size = pq_topk.size();
    while(total_size>=0)                            // no parallelization possible..
    {
        thistopk[total_size] = pq_topk.top().second;
        pq_topk.pop();
        total_size--;
    }
    return;
}

int main(int argc, char* argv[]){
    assert(argc>4);

    string out_dir = argv[1];
    int k = stoi(argv[2]);              // check if needs to use uint32_t
    string user_file = argv[3];         //embeddings..
    string out_pred = argv[4];

    int temp;
    int max_level,ep;
    vector<int> level;
    vector<int> level_offset;
    vector<int> indptr;
    vector<int> index;
    vector<vector<float>> vect;
    vector<vector<float>> userEmbed;

    
    // read all the files appropriately..
    fstream outfil(out_dir+"/max_level.bin",ios::in);
    outfil.read((char*)&max_level,4);
    outfil.close();

    outfil.open(out_dir+"/ep.bin",ios::in);
    outfil.read((char*)&ep,4);
    outfil.close();

    outfil.open(out_dir+"/level.bin",ios::in);
    while(outfil.read((char*)&temp,4))
    {
        level.push_back(temp);
    }
    outfil.close();
    
    outfil.open(out_dir+"/level_offset.bin",ios::in);
    while(outfil.read((char*)&temp,4))
    {
        level_offset.push_back(temp);
    }
    outfil.close();

    outfil.open(out_dir+"/indptr.bin",ios::in);
    while(outfil.read((char*)&temp,4))
    {
        indptr.push_back(temp);
    }
    outfil.close();
    
    outfil.open(out_dir+"/index.bin",ios::in);
    while(outfil.read((char*)&temp,4))
    {
        index.push_back(temp);
    }
    outfil.close();

    outfil.open(out_dir+"/vect.bin",ios::in);
    int size;
    outfil.read((char*)&size,4);
    float tmp;
    vector<float> teemp;
    int i=0;
    while(outfil.read((char*)&tmp,sizeof(float)))
    {
        teemp.push_back(tmp);
        i++;
        if(i == size)
        {
            i = 0;
            vect.push_back(teemp);
            teemp.resize(0);
        }
    }
    outfil.close();

    outfil.open(user_file,ios::in);
    i = 0;
    teemp.resize(0);
    while(outfil.read((char*)&tmp,sizeof(float)))
    {
        teemp.push_back(tmp);
        i++;
        if(i == size)
        {
            i = 0;
            userEmbed.push_back(teemp);
            teemp.resize(0);
        }
    }
    outfil.close();

    vector<vector<int>> outputK(userEmbed.size(),vector<int> (k,-1));        //recommendation==-1 means not yet computed.

    for(int idx=0;idx<userEmbed.size();idx++)
    {
        QueryHNSW(userEmbed[idx],outputK[idx],ep,indptr,index,level_offset,max_level,vect);
        for(int i=0;i<k;i++)
        {
            cout << outputK[idx][i] << " ";
        }
        cout << "\n";
    }

}