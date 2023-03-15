#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <unordered_map>
#include "string.h"
using namespace std;

vector<int> GetRelatieEssenCubes(vector<vector<int>> cubelist);
vector<int> GetRedudantCubes(vector<vector<int>> cubelist, vector<int> RelativeEssencol);
bool URPCubeContain(vector<vector<int>> cubelist, int termid);
bool URPTautologyCheck(vector<vector<int>> cubelist, unordered_map<int, int> selectflag);
vector<vector<int>> CubelistAssign(vector<vector<int>> cubelist, int col_chosen, int pos_flag);
bool JudgeAllDCCube(vector<vector<int>> cubelist);
bool JudgeUnate(vector<vector<int>> cubelist);
bool JudgeColumnUnate(vector<int> cubelistColumn);
bool JudgeSingleVarPolar(vector<vector<int>> cubelist);
int FindMostBinateVar(vector<vector<int>> cubelist, unordered_map<int,int> selectflag);


int main(int argc, char* argv[])
{
    if(argc<3 || argc>4)
    {
        cout<<"usage: reduce <in_file> <out_file> [unate_reduce_log_file]"<<endl;
    }
    else
    {
        string in_file_name = argv[1];
        string out_file_name = argv[2];
        string unate_reduct_file_name = (argc==4? argv[3]:"");

        //cout<<in_file_name<<endl;
        ifstream fin(in_file_name);
        string line;

        getline(fin, line);


        int num_variable = stoi(line);

        cout<<num_variable<<endl;

        getline(fin, line);

        int num_term = stoi(line);

        cout<<num_term<<endl;

        vector<vector<int>> cubelist(num_term, vector<int>(num_variable, 0));
        for(int i=0; i<num_term; i++)
        {
            getline(fin, line);
            for(int j=0; j<num_variable; j++)
            {
                cubelist[i][j] = int(line[j]-'0');
                cout<<cubelist[i][j]<<' ';
            }
            cout<<endl;
        }

        vector<int> RelativeEssencol;
        vector<int> Redundantcol;

        RelativeEssencol = GetRelatieEssenCubes(cubelist);
        /*
        for(int i=0; i<RelativeEssencol.size(); i++)
            cout<<RelativeEssencol[i]<<' ';
        cout<<endl;
        */

        Redundantcol = GetRedudantCubes(cubelist, RelativeEssencol);
        /*
        for(int i=0; i<Redundantcol.size(); i++)
            cout<<Redundantcol[i]<<' ';
        cout<<endl;
        */

        unordered_map<int, int> Redundantmap;
        for(int i=0; i<Redundantcol.size(); i++)
            Redundantmap[Redundantcol[i]]= 1;
        
        vector<vector<int>> cubelistremain;
        for(int i=0; i<cubelist.size(); i++)
        {
            if(Redundantmap.find(i)==Redundantmap.end())
                cubelistremain.push_back(cubelist[i]);
        }

        cout<<cubelistremain.size()<<endl;

        for(int i=0; i<cubelistremain.size(); i++)
        {
            for(int j=0; j<cubelistremain[i].size(); j++)
                cout<<cubelistremain[i][j]<<' ';
            cout<<endl;
        }

    }

    return 0;
}

vector<int> GetRelatieEssenCubes(vector<vector<int>> cubelist)
{
    vector<int> RelativeEssencol;
    for(int i=0; i<cubelist.size(); i++)
    {
        if(!URPCubeContain(cubelist, i))
        {   //cout<<'d'<<endl;
            RelativeEssencol.push_back(i);
        }
    }
    return RelativeEssencol;
    
}

vector<int> GetRedudantCubes(vector<vector<int>> cubelist, vector<int> RelativeEssencol)
{
    unordered_map<int, int> RelativeEssencolmap;
    vector<vector<int>> cubelistnew;

    for(int i=0; i<RelativeEssencol.size(); i++)
    {
        RelativeEssencolmap[RelativeEssencol[i]] = 1;
        cubelistnew.push_back(cubelist[RelativeEssencol[i]]);
    }
    
    vector<int> Redundantcol;

    for(int i=0; i<cubelist.size(); i++)
    {
        if(RelativeEssencolmap.find(i) == RelativeEssencolmap.end())
        {
            cubelistnew.push_back(cubelist[i]);
            if(URPCubeContain(cubelistnew, cubelistnew.size()-1))
                Redundantcol.push_back(i);
            cubelistnew.pop_back();
        }
    }

    return Redundantcol;
}


bool URPCubeContain(vector<vector<int>> cubelist, int termid)
{
    vector<vector<int>> cubelistnew;
    unordered_map<int, int> cubedeleteidmap;
    bool Containflag = false;
    for(int i=0; i<cubelist.size(); i++)
    {
        if(i==termid)
            continue;

        for(int j=0; j<cubelist[0].size(); j++)
        {
            if(cubelist[termid][j]==2 || cubelist[i][j] == 2)
                continue;
            else if(cubelist[termid][j] == 0)
            {
                if(cubelist[i][j] == 0)
                    cubelist[i][j] = 2;
                else
                {
                    if(cubedeleteidmap.find(i) == cubedeleteidmap.end())
                        cubedeleteidmap[i] = 1;
                }
            }
            else
            {
                if(cubelist[i][j]==1)
                    cubelist[i][j] = 2;
                else
                {
                    if(cubedeleteidmap.find(i) == cubedeleteidmap.end())
                        cubedeleteidmap[i] = 1;                    
                }
            }
            
        }
    }

    for(int i=0; i<cubelist.size(); i++)
    {
        if(i==termid)
            continue;
        if(cubedeleteidmap.find(i) == cubedeleteidmap.end())
            cubelistnew.push_back(cubelist[i]);
    }
    
    /*
    cout<<endl;
    for(int i=0; i<cubelistnew.size(); i++)
    {
        for(int j=0; j<cubelistnew[0].size(); j++)
            cout<<cubelistnew[i][j]<<' ';
        cout<<endl;
    }
    */

    unordered_map<int,int> selectflag;
    Containflag = URPTautologyCheck(cubelistnew, selectflag);
    //cout<<Containflag<<endl;
    //cout<<'d'<<endl;

    return Containflag;
}

bool URPTautologyCheck(vector<vector<int>> cubelist, unordered_map<int, int> selectflag)
{
    if(cubelist.size() == 0)
        return false;

    if(JudgeAllDCCube(cubelist))
        return true;
    
    if(JudgeUnate(cubelist))
        return false;
    
    if(JudgeSingleVarPolar(cubelist))
        return true;
    
    int col_chosen = FindMostBinateVar(cubelist, selectflag);
    selectflag[col_chosen] = 1;
    //cout<<col_chosen<<endl;
    //cout<<'b'<<endl;
    
    vector<vector<int>> cubelistpos;
    vector<vector<int>> cubelistneg;
    cubelistpos = CubelistAssign(cubelist, col_chosen, 1);
    cubelistneg = CubelistAssign(cubelist, col_chosen, 0);

    //cout<<cubelistpos.size()<<endl;
    //cout<<cubelistneg.size()<<endl;

    return URPTautologyCheck(cubelistpos, selectflag) && URPTautologyCheck(cubelistneg, selectflag);
}

vector<vector<int>> CubelistAssign(vector<vector<int>> cubelist, int col_chosen, int pos_flag)
{
    vector<vector<int>> cubelistnew;
    unordered_map<int, int> cubedeleteidmap;

    for(int i=0; i<cubelist.size(); i++)
    {
        if(cubelist[i][col_chosen] == 2)
            continue;
        else if(cubelist[i][col_chosen]==1)
        {
            if(pos_flag)
                cubelist[i][col_chosen] = 2;
            else
                cubedeleteidmap[i] = 1;
        }
        else
        {
            if(pos_flag)
                cubedeleteidmap[i] = 1;
            else
                cubelist[i][col_chosen] = 2;
        }
    }

    for(int i=0; i<cubelist.size(); i++)
    {
        if(cubedeleteidmap.find(i) == cubedeleteidmap.end())
            cubelistnew.push_back(cubelist[i]);
    }


    /*
    for(int i=0; i<cubelistnew.size(); i++)
    {
        for(int j=0; j<cubelistnew[i].size(); j++)
            cout<<cubelistnew[i][j]<<' ';
        cout<<endl;
    }

    cout<<endl;
    */
    

    return cubelistnew;
}

bool JudgeAllDCCube(vector<vector<int>> cubelist)
{
    bool flag_all_dc;
    
    for(int i=0; i<cubelist.size(); i++)
    {
        flag_all_dc = true;
        for(int j=0; j<cubelist[0].size(); j++)
        {
            if(cubelist[i][j] != 2)
            {
                flag_all_dc = false;
                break;
            }
        }

        if(flag_all_dc)
            return true;
    }

    return false;
}

bool JudgeUnate(vector<vector<int>> cubelist)
{
    vector<int> cubelistColumn;
    for(int i=0; i<cubelist[0].size(); i++)
    {
        //cout<<i<<endl;
        cubelistColumn.clear();
        for(int j=0; j<cubelist.size(); j++)
            cubelistColumn.push_back(cubelist[j][i]);
        if(!JudgeColumnUnate(cubelistColumn))
            return false;
    }

    return true;

}

bool JudgeColumnUnate(vector<int> cubelistColumn)
{
    unordered_map<int, int> valuemap;
    vector<int> value;

    for(int i=0; i<cubelistColumn.size(); i++)
    {
        if(valuemap.find(cubelistColumn[i])==valuemap.end())
        {
            valuemap[cubelistColumn[i]] = 1;
            value.push_back(cubelistColumn[i]);
        }
    }

    if(value.size()<=1)
        return true;
    else if(value.size()>2)
        return false;
    else
    {
        if(value[0]==2 || value[1]==2)
            return true;
        else
            return false;
    }
}

bool JudgeSingleVarPolar(vector<vector<int>> cubelist)
{
    unordered_map<int, int> singlevarIDmap;

    int count_dc;
    int last_non_dc;

    for(int i=0; i<cubelist.size(); i++)
    {
        count_dc = 0;
        for(int j=0; j<cubelist[0].size(); j++)
        {
            if(cubelist[i][j]==2)
                count_dc++;
            else
                last_non_dc = j;
        }

        if(count_dc == cubelist[0].size()-1)
        {
            if(singlevarIDmap.find(last_non_dc)!=singlevarIDmap.end())
            {
                if(singlevarIDmap[last_non_dc]==1-cubelist[i][last_non_dc])
                    return true;
            }
            else
                singlevarIDmap[last_non_dc] = cubelist[i][last_non_dc];
        }
    }
    return false;
}

int FindMostBinateVar(vector<vector<int>> cubelist, unordered_map<int,int> selectflag)
{
    int num_zero;
    int num_one;
    int most_binate_metric = cubelist.size();
    int most_binate_col = 0;

    for(int i=0; i<cubelist[0].size(); i++)
    {
        if(selectflag.find(i)!=selectflag.end())
            continue;
        num_zero = 0;
        num_one = 0;
        for(int j=0; j<cubelist.size(); j++)
        {
            if(cubelist[j][i] == 0)
                num_zero++;
            else if(cubelist[j][i] == 1)
                num_one++;
        }

        if(abs(num_one-num_zero)<most_binate_metric)
        {
            most_binate_metric = abs(num_one-num_zero);
            most_binate_col = i;
        }
    }

    return most_binate_col;
}