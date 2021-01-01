#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX_GEN      30  //最大世代交替
#define POP_SIZE     12  //集団のサイズ
#define LEN_CHROM    32*3//一個体の遺伝子の長さ
#define GEN_GAP      0.2 //世代交替の割合
#define P_CROSS      0.5 //一様交叉率
#define P_MUTAION    0. //突然変異の確率
#define RANDOM_MAX   32767
#define BEFORE       0
#define AFTER        1

char chrom[POP_SIZE][LEN_CHROM]; //染色体
int fitness[POP_SIZE];          //適合度
int sort_chrom[POP_SIZE];       //選択された親個体の要素番号（高い順）
int max, min, sumfitness;       //適合度のmax,min,sum

//引数iに対応する個体の遺伝子情報と適合度を表示
PrintEachChromFitness(int i){
    int j;
    printf("個体%d : %d\n", i, fitness[i]);
    for(j=0; j<LEN_CHROM; j++){
        if((j%32 == 0) && (j != 0))  printf("\n");  //セルが32個毎に改行
        printf("%2d ", chrom[i][j]);
    }
    printf("\n");
}

//個体ごとに遺伝子情報と適合度を表示する関数を呼び出す
PrintChromFitness(){
    int i;
    for(i=0; i<POP_SIZE; i++){
        PrintEachChromFitness(i);
    }
}

//世代数，適合度の最大値，最小値，合計値，平均値を表示
PrintStatistics(int gen){
    double ave;
    ave = (double)sumfitness/(double)POP_SIZE
    printf("[gen=%2d] max=%d min=%d sumfitness=%d ave=%f\n", gen, max, min, sumfitness, ave);
}

//選択された親，除去された個体，新たな子供のそれぞれの遺伝子情報と適合度を表示する関数を呼び出す
PrintCrossover(int flag, int parent1, int parent2, int child1, int child2){
    int i;
    
    switch(flag){
        case BEFORE:
            printf("<parent1> ");
            PrintEachChromFitness(parent1);
            printf("<parent2> ");
            PrintEachChromFitness(parent2);
            printf("<delete1> ");
            PrintEachChromFitness(child1);
            printf("<delete2> ");
            PrintEachChromFitness(child2);
            break;
        case AFTER:
            //交叉位置を表示
            printf("交叉位置:\n")
            for(i=0; i<LEN_CHROM; i++){
                if((i%32 == 0) && (i != 0))  printf("\n");  //セルが32個毎に改行
                printf("%2d ", n_cross[i]);
            }
            printf("\n");
            printf("<child1> ");
            PrintEachChromFitness(child1);
            printf("<child2> ");
            PrintEachChromFitness(child2);
            printf("-----------------------------------------------------------------\n");
            break;
    }
}

//
PrintMutation(int flag, int child, int n_mutate){
    switch(flag){
        case BEFORE:
            printf("child(OLD)");
    }
}

//第1世代の処理
Generation(int gen){
    int parent1, parent2; //親の要素数を格納
    int child1, child2;   //子供の要素数を格納
    int n_gen = 3;
    int n_delete = 7;     //子供に置き換える除去する個体の要素数を格納
    int i, j;

    //集団の表示
    Statistics();
    PrintStatistics(gen);

    //世代交替（？）
    Statistics();
    Select();   //どっちかというとソート
    for(i=0; i<(n_gen-1); i++){     //(1,2),(1,3),(2,3)の順で子供*2を作る
       for(j=i+1; j<n_gen; j++) 
          parent1 = sort_chrom[i];  //適応度の高い個体の要素順に並べたsort_chromから取り出す
          parent2 = sort_chrom[j];  //上記に同じ
          child1 = n_delete;         //適応度が7～12番目の個体を新しい子供で置き換えていく
          child2 = n_delete+1;
          Crossover(parent1, parent2, child1, child2);
          Mutation(child1);
          Mutation(child2);

          n_delete = n_delete + 2;
    }
}

//選択（選択の変更点memoのファイルを同じとこに置いてるよ）
void Select(){
    int i, j;
    int n_max;  //適応度が最大の個体の要素を格納

    //個体のソート，適応度の高い順（選択ソート）
    for(i=0; i<(POP_SIZE-1); i++){
        n_max = 0;
        for(j=i; j<(POP_SIZE-1); j++){
            if(fitness[j]>n_max)  n_max = j; //適応度が最大の個体の要素数を探索
        }
        sort_chrom[i] = n_max; //適応度が高い個体の要素数順に格納
    }
}

//交叉
void Crossover(int parent1, int parent2, int child1, int child2){
    int i;
    int n_cross[LEN_CHROM] = {0};   //交叉位置を格納

    //交叉
    PrintCrossover(BEFORE, parent1, parent2, child1, child2, n_cross);

    for(i=0; i<LEN_CHROM; i++){
        if((double)rand()/RAND_MAX < P_CROSS){  //確率0.5で交叉
            chrom[child1][i] = chrom[parent2][i];
            chrom[child2][i] = chrom[parent1][i];
            n_cross[i] = 1; //交叉した位置を格納
        } else {                                //それ以外は交叉せずそのまま格納
            chrom[child1][i] = chrom[parent1][i];
            chrom[child2][i] = chrom[parent2][i];
        }
    }
    //各子供の適合度を格納
    fitness[child1] = ObjFunc(child1);
    fitness[child2] = ObjFunc(child2);

    PrintCrossover(AFTER, parent1, parent2, child1, child2, n_cross);
}

//突然変異
Mutation(int child){
    int n_mutate;
    int scale;
    double random;

    random = (double)rand()/(double)RAND_MAX; // 0<=random<1
    if(rand < P_MUTATION){  //確率5%未満なら突然変異
        //突然変異位置（n_mutate=0,...,96）
        n_mutate = rand()%LEN_CHROM;

        //突然変異
        PrintMutation(BEFORE, child, n_mutate);
        //scale = rand() % (音階格納する配列の要素数(24+1)もしくはその変数)　（乱数か次の音階）
        //chrom[child][n_mutate] = 音階格納する配列[scale];
        fitness[child] = ObjFunc(child);
        PrintMutation(AFTER, child, n_mutate);
    }
}
