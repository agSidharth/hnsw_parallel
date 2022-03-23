#include <bits/stdc++.h>
#include <mpi.h>
#include <omp.h>

using namespace std;

class comp
{
    public:
    int operator() (const pair<float,int> &a,const pair<float,int> &b)
    {
        if(b.first != a.first)
            return b.first < a.first;
        else
            return b.second > a.second;
    }
};

float cosine_dist(vector<float>& x,vector<float>& y)          // can be parallelized
{
    float modX,modY,dotXY;
    modX = modY = dotXY = 0;
    for(int idx=0;idx<x.size();idx++)
    {
        modX += x[idx]*x[idx];
        modY += y[idx]*y[idx];
        dotXY += x[idx]*y[idx];
    }

    if(modX == 0 || modY == 0) return 0;
    return  float(dotXY)/float(sqrt(modX)*sqrt(modY));
}

void SearchLayer(vector<float>& q,priority_queue<pair<float,int>, vector<pair<float,int>>,comp>& topk,vector<int>& indptr,vector<int>& index,vector<int>& level_offset,int lc,vector<int>& visited,vector<vector<float>>& vect,int k)
{
    priority_queue<pair<float,int>, vector<pair<float,int>>,comp> candidates = topk;          //confirm if it is a priority queue or not..

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

            if (_dist <= topk.top().first && topk.size()==k) continue;

            if(topk.size()==k) topk.pop();
            topk.push({_dist,px});
            candidates.push({_dist,px});
        }
    }
}


void QueryHNSW(vector<float>& q,vector<int>& thistopk,int ep,vector<int>& indptr,vector<int>& index,vector<int>& level_offset,int max_level,vector<vector<float>>& vect)
{

    priority_queue<pair<float,int>, vector<pair<float,int>>,comp> pq_topk;          //store (distance,node_id)
    pq_topk.push({cosine_dist(q,vect[ep]),ep});   
    vector<int> visited(vect.size(),0);

    visited[ep] = 1;
    for(int lev=max_level;lev>=0;lev--)               // no parallelization possible..
    {
        SearchLayer(q,pq_topk,indptr,index,level_offset,lev,visited,vect,thistopk.size());
    }

    int total_size = pq_topk.size();
    while(total_size>0)                            // no parallelization possible..
    {
        thistopk[total_size-1] = pq_topk.top().second;
        //cout << "  a  " << pq_topk.top().second << ":" << pq_topk.top().first <<" ";
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
    while(outfil>>tmp)
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

    int rank, sze;
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &sze);
    #pragma omp parallel for num_threads(4)
    for(int idx=rank;idx<userEmbed.size();idx+=sze)
    {
        QueryHNSW(userEmbed[idx],outputK[idx],ep,indptr,index,level_offset,max_level,vect);
        //cout << "1" <<endl;
    }
    
    //cerr<<"rank: "<<rank<<", size: "<<sze<<endl;

    if(rank%sze==0)
    {
        for(int mdx=0;mdx<outputK.size();mdx++)
        {
            if(mdx%sze==0) continue;

            /*
            cerr<<"index: "<<mdx<<endl;
            for(int tdx=0;tdx<outputK[mdx].size();tdx++) cerr<<outputK[mdx][tdx]<<" ";
            */

            MPI_Recv(&outputK[mdx][0],k,MPI_INT,mdx%sze,mdx,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

            /*
            cerr<<"\n";
            for(int tdx=0;tdx<outputK[mdx].size();tdx++) cerr<<outputK[mdx][tdx]<<" ";
            cerr<<"\n";
            */
        }
    }
    else
    {
        for(int mdx=rank;mdx<outputK.size();mdx += sze)
        {
            MPI_Send(&outputK[mdx][0],k,MPI_INT,0,mdx,MPI_COMM_WORLD);
        }
    }

    if(rank==0)
    {
        for(int idx=0;idx<outputK.size();idx++)
        {
            cerr<<"index: "<<idx<<" : ";
            for(int fdx=0;fdx<outputK[idx].size();fdx++) cerr<<outputK[idx][fdx]<<" ";
            cerr<<"\n";
        }
    }

    MPI_Finalize();
}