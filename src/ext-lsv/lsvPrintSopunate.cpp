#include "base/abc/abc.h"
#include "base/main/main.h"
#include "base/main/mainInt.h"

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>

using namespace std;

int Lsv_CommandPrintSopunate(Abc_Frame_t* pAbc, int argc, char** argv);
void lsv_print_sopunate_ctor(Abc_Frame_t* pAbc){
	Cmd_CommandAdd(pAbc, "Print LSV Sopunate", "lsv_print_sopunate", Lsv_CommandPrintSopunate, 0);
}

void lsv_print_sopunate_dtor(Abc_Frame_t* pAbc){}

Abc_FrameInitializer_t lsv_print_sopunate_initializer = {
	lsv_print_sopunate_ctor,
	lsv_print_sopunate_dtor
};

struct registrator_lsv_print_sopunate{
	registrator_lsv_print_sopunate(){
		Abc_FrameAddInitializer(&lsv_print_sopunate_initializer);
	}
	Abc_Obj_t* pObj;
} registrate_lsv_print_sopunate;


struct fi{
	unsigned int id;
	string name;
	char unateness; // 1:pos, 0:neg, -:binate, u:undecided
};

int Lsv_CommandPrintSopunate(Abc_Frame_t* pAbc, int argc, char** argv){
	Abc_Ntk_t* pNtk = Abc_FrameReadNtk(pAbc);
	if(!pNtk){
		Abc_Print(-1, "Empty Network.\n");
		return 1;
	}
	if (!Abc_NtkHasSop(pNtk)) return 1;
	Abc_Obj_t* pObj;
	int i;
	Abc_NtkForEachNode(pNtk, pObj, i){
    char* pSop = (char*)Abc_ObjData(pObj);
		int nVars = Abc_SopGetVarNum(pSop);
		if(nVars == 0) continue;
		cout << "node " << Abc_ObjName(pObj) << ":" << endl;
		vector<fi> fi_s;
		
		Abc_Obj_t *pFanin;
		int j;
		Abc_ObjForEachFanin(pObj, pFanin, j)
			fi_s.push_back({.id=Abc_ObjId(pFanin), .name=Abc_ObjName(pFanin), .unateness='u'});

		char* pCube;
		Abc_SopForEachCube((char*)pObj->pData, nVars, pCube) {
			char unateness;
			Abc_CubeForEachVar(pCube, unateness, j){
				char& old_unateness = fi_s[j].unateness;
				if(unateness == '1' || unateness == '0'){
					if(old_unateness == 'u') old_unateness = unateness;
					else if(old_unateness != '-' && old_unateness != unateness)
						old_unateness = '-';
				}
			}
		}
		sort(fi_s.begin(), fi_s.end(), [](const fi &a, const fi &b){
			return a.id <= b.id;
		});
    bool phase = Abc_SopGetPhase(pSop);
		vector<string> unate_arr[3]; // 0:+unate, 1:-unate, 2:binate
		for(auto& fi_ins : fi_s){
			if(fi_ins.unateness == '-')
				unate_arr[2].push_back(fi_ins.name);
			else if(fi_ins.unateness == 'u'){
				unate_arr[0].push_back(fi_ins.name);
				unate_arr[1].push_back(fi_ins.name);
      } else
				unate_arr[(fi_ins.unateness-'0') != phase].push_back(fi_ins.name);
		}
		for(j=0; j<3; ++j){
			if(unate_arr[j].empty()) continue;
			switch(j){
				case 0:  cout << "+u"; break;
				case 1:  cout << "-u"; break;
				case 2:  cout << "bi"; break;
			}
			cout << "nate inputs: ";
			auto print_names = [](vector<string>& unate_arr_j){
				cout << unate_arr_j[0];
				for(int k=1; k<unate_arr_j.size(); ++k)
					cout << ',' << unate_arr_j[k];
				cout << endl;
			};
			print_names(unate_arr[j]);
		}
	}
	return 0;
}
