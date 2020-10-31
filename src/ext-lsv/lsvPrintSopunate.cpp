#include "base/abc/abc.h"
#include "base/main/main.h"
#include "base/main/mainInt.h"

static int Lsv_CommandPrintSopunate(Abc_Frame_t* pAbc, int argc, char** argv);

void Lsv_InitPrintSopunate(Abc_Frame_t* pAbc) {
  Cmd_CommandAdd(pAbc, "LSV", "lsv_print_sopunate", Lsv_CommandPrintSopunate, 0);
}

void Lsv_DestroyPrintSopunate(Abc_Frame_t* pAbc) {}

Abc_FrameInitializer_t Lsv_FrameInitPrintSopunate = {Lsv_InitPrintSopunate, Lsv_DestroyPrintSopunate};

struct Fanins_t{
  int i;
  unsigned int FaninId;
};

int comp(const void *a, const void *b){
  return ((Fanins_t*)a)->FaninId > ((Fanins_t*)b)->FaninId;
}

void Lsv_NtkPrintSopunate(Abc_Ntk_t* pNtk) {
  if (!Abc_NtkHasSop(pNtk)) return;
  Abc_Obj_t* pObj;
  int i;
  Abc_NtkForEachNode(pNtk, pObj, i) {
    char* pSop = (char*)Abc_ObjData(pObj);
    int nFanins = Abc_SopGetVarNum(pSop);

    // Some testcase will have nodes not present in BLIF with nFanins = 0 print at end of the output, but don't know what does these node represents
    // Skip such nodes
    if(nFanins == 0) continue;
    // Not printing this line when nFanins == 0
    printf("node %s:\n", Abc_ObjName(pObj));

    // Initialize array that stores unateness of fanins
    char Values[nFanins];
    for(int i=0; i<nFanins; ++i)
      Values[i] = '-';

    // Calculate unateness by reduction over cubes
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
    
    // Flip unateness if Sop if offset
    if(Abc_SopGetPhase(pSop) == 0)
      for(int i=0; i<nFanins; ++i)
        if(Values[i]=='0' || Values[i] =='1')
          Values[i] = !(Values[i]-'0') +'0';

    //[Abc_ObjFaninNum(pObj)];
    // Array that Fanins[j].i maps sop fanin number j to node FaninId i
    Fanins_t Fanins[nFanins]; // Is it correct that all fanin of node is same as all fanins of cube?

    Abc_Obj_t* pFanin;
    int j;
    Abc_ObjForEachFanin(pObj, pFanin, j)
      Fanins[j] = {j, Abc_ObjId(pFanin)};

    qsort(Fanins, nFanins, sizeof(*Fanins), comp);
    
    // Printing unateness by traverse by FaninId nondecreasing
    bool first = true;
    for(int j=0; j<nFanins; ++j){
      int i = Fanins[j].i;
      if(Values[i] == '1' || Values[i] == '-'){
        if(first){
          printf("+unate inputs: ");
          first = false;
        } else
          printf(",");
        printf("%s", Abc_ObjName(Abc_ObjFanin(pObj, i)));
      }
    }
    if(!first)
      printf("\n");
    
    first = true;
    for(int j=0; j<nFanins; ++j){
      int i = Fanins[j].i;
      if(Values[i] == '0' || Values[i] == '-'){
        if(first){
          printf("-unate inputs: ");
          first = false;
        } else
          printf(",");
        printf("%s", Abc_ObjName(Abc_ObjFanin(pObj, i)));
      }
    }
    if(!first)
      printf("\n");

    first = true;
    for(int j=0; j<nFanins; ++j){
      int i = Fanins[j].i;
      if(Values[i] == 'b'){
        if(first){
          printf("binate inputs: ");
          first = false;
        } else
          printf(",");
        printf("%s", Abc_ObjName(Abc_ObjFanin(pObj, i)));
      }
    }
    if(!first)
      printf("\n");
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
  Abc_Print(-2, "\t        prints sopunate for each node in the network\n");
  Abc_Print(-2, "\t-h    : print the command usage\n");
  return 1;
}
