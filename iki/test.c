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

int chrom[POP_SIZE][LEN_CHROM]; //染色体
int fitness[POP_SIZE];          //適合度
int sort_chrom[POP_SIZE];       //選択された親個体の要素番号（高い順）
int max, min, sumfitness;       //適合度のmax,min,sum

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

    //交叉
    PrintCrossover(BEFORE, parent1, parent2, child1, child2);

    for(i=0; i<LEN_CHROM; i++){
        if((double)rand()/RAND_MAX < P_CROSS){  //確率0.5で交叉
            chrom[child1][i] = chrom[parent2][i];
            chrom[child2][i] = chrom[parent1][i];
        } else {                                //それ以外は交叉せずそのまま格納
            chrom[child1][i] = chrom[parent1][i];
            chrom[child2][i] = chrom[parent2][i];
        }
    }
    //各子供の適合度を格納
    fitness[child1] = ObjFunc(child1);
    fitness[child2] = ObjFunc(child2);

    PrintCrossover(AFTER, parent1, parent2, child1, child2);
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
        //scale = rand() % (音階格納する配列の要素数(24+1)もしくはその変数)
        //chrom[child][n_mutate] = 音階格納する配列[scale]
    }
}
