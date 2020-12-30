%%file Evolutionary_computation.c

#include <stdio.h>
#include <math.h>

#define MAX_GEN      30  //最大世代交替
#define POP_SIZE     12  //集団のサイズ
#define LEN_CHROM    32*3//一個体の遺伝子の長さ
#define GEN_GAP      0.2 //世代交替の割合
#define P_CROSS      0.5 //一様交叉率
#define P_MUTAION    0.1 //突然変異の確率
#define RANDOM_MAX   32767
#define BEFORE       0
#define AFTER        1

int chrom[POP_SIZE][LEN_CHROM];  //染色体
int fitness[POP_SIZE];           //適合度
int sort_fitness[POP_SIZE];     //選択された親個体の要素番号（昇順）
int max, min, sumfitness;        //適合度のmax,min,sum
int n_min;                       //適合度のminの添え字

//第1世代の処理
Generation(int gen){
    int parent1, parent2;
    int child1, child2;
    int n_gen;
    int i, j;

    //集団の表示
    Statistics();
    PrintStatistics(gen);

    //世代交替
    n_gen = 4
    for(i=0; i<n_gen; i++){
       for(j=1; j<n_gen; j++) 
          Statistics();
          Select();
          parent1 = select_parent[i];
          parent2 = select_parent[j];
          Crossover(parent1, parent2, &child1, &child2);
          Mutation(child1);
          Mutation(child2);
    }
}

//選択
void Select(){
    int i, j;
    int max;

    max = 0;
    //個体のソート，適応度の高い順
    for(i=0; i<(POP_SIZE-1); i++){
        for(j=i; j<(POP_SIZE-1); j++){
            if(fitness[j]>max)  max = j; //適応度が最大の個体の要素を探索
        }
        sort_fitness[i] = max; //適応度が最大の個体の要素を格納
    }
}

//交叉
void Crossover(int parent1, int parent2, int *child1, child2){
    int min 2;
    int n_cross[LEN_CHROM] = {};
    int i;
    
    //交叉セルの決定
    for(i=0; i<LEN_CHROM; i++){
        if( (double)rand()/RANDOM_MAX > 0.5)    n_cross[i] = 1;  //確率が0.5の場合，i番目のセルを1に
    }
}
