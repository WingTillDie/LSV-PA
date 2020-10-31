#include "base/abc/abc.h"
#include "base/main/main.h"
#include "base/main/mainInt.h"

static int Lsv_CommandPrintSopunate(Abc_Frame_t* pAbc, int argc, char** argv);

void Lsv_InitPrintSopunate(Abc_Frame_t* pAbc) {
  Cmd_CommandAdd(pAbc, "LSV", "lsv_print_sopunate", Lsv_CommandPrintSopunate, 0);
}

void Lsv_DestroyPrintSopunate(Abc_Frame_t* pAbc) {}

Abc_FrameInitializer_t Lsv_FrameInitPrintSopunate = {Lsv_InitPrintSopunate, Lsv_DestroyPrintSopunate};

void Lsv_NtkPrintSopunate(Abc_Ntk_t* pNtk) {
  Abc_Obj_t* pObj;
  int i;
  Abc_NtkForEachNode(pNtk, pObj, i) {
    //printf("Object Id = %d, name = %s\n", Abc_ObjId(pObj), Abc_ObjName(pObj));
    printf("node %s:\n", Abc_ObjName(pObj));
    /*
    Abc_Obj_t* pFanin;
    int j;
    Abc_ObjForEachFanin(pObj, pFanin, j) {
      printf("  Fanin-%d: Id = %d, name = %s\n", j, Abc_ObjId(pFanin),
             Abc_ObjName(pFanin));
    }
    */
    /*
    if (Abc_NtkHasSop(pNtk)) {
      printf("The SOP of this node:\n%s", (char*)pObj->pData);
    }
    */
    char* pSop;
    if (!Abc_NtkHasSop(pNtk)) return;
    else pSop = (char*)Abc_ObjData(pObj);
    int nFanins = Abc_SopGetVarNum(pSop);

    char Values[nFanins+1];
    for(int i=0; i<nFanins; ++i)
      Values[i] = '-';
    Values[nFanins] = '\0';

    char* pCube;
    Abc_SopForEachCube( pSop, nFanins, pCube ){
      int i;
      char Value;
      Abc_CubeForEachVar( pCube, Value, i )
        if(Values[i] == '-')
          Values[i] = Value;
        else if ((Values[i]=='0' && Value=='1') || (Values[i]=='1' && Value=='0'))
          Values[i] = 'b'; // Binate
    }
    
    if(Abc_SopGetPhase(pSop) == 0)
      for(int i=0; i<nFanins; ++i)
        if(Values[i]=='0' || Values[i] =='1')
          Values[i] = !(Values[i]-'0') +'0';
    
    bool first = true;
    for(int i=0; i<nFanins; ++i)
      if(Values[i] == '1' || Values[i] == '-'){
        //printf("%d,", i);
        if(first){
          printf("+unate inputs: ");
          first = false;
        } else
          printf(",");
        printf("%s", Abc_ObjName(Abc_ObjFanin(pObj, i)));
      }
    if(!first)
      printf("\n");
    
    first = true;
    for(int i=0; i<nFanins; ++i)
      if(Values[i] == '0' || Values[i] == '-'){
        if(first){
          printf("-unate inputs: ");
          first = false;
        } else
          printf(",");
        printf("%s", Abc_ObjName(Abc_ObjFanin(pObj, i)));
      }
    if(!first)
      printf("\n");

    first = true;
    for(int i=0; i<nFanins; ++i)
      if(Values[i] == 'b'){
        if(first){
          printf("binate inputs: ");
          first = false;
        } else
          printf(",");
        printf("%s", Abc_ObjName(Abc_ObjFanin(pObj, i)));
      }
    if(!first)
      printf("\n");
    // TODO Traverse by ObjId
  }
}

int Lsv_CommandPrintSopunate(Abc_Frame_t* pAbc, int argc, char** argv) {
  Abc_Ntk_t* pNtk = Abc_FrameReadNtk(pAbc);
  int c;
  Extra_UtilGetoptReset();
  while ((c = Extra_UtilGetopt(argc, argv, "h")) != EOF) {
    switch (c) {
      case 'h':
        goto usage;
      default:
        goto usage;
    }
  }
  if (!pNtk) {
    Abc_Print(-1, "Empty network.\n");
    return 1;
  }
  Lsv_NtkPrintSopunate(pNtk);
  return 0;

usage:
  Abc_Print(-2, "usage: lsv_print_sopunate [-h]\n");
  //Abc_Print(-2, "\t        prints the nodes in the network\n");
  Abc_Print(-2, "\t        prints sopunate for each node in the network\n");
  Abc_Print(-2, "\t-h    : print the command usage\n");
  return 1;
}
