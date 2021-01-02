#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX_GEN      30  //最大世代交替
#define POP_SIZE     12  //集団のサイズ
#define LEN_CHROM    32*3//一個体の遺伝子の長さ
#define LEN_SOUND    24+1 //音数(無音は0)
#define GEN_GAP      0.2 //世代交替の割合
#define P_CROSS      0.5 //一様交叉率
#define P_MUTATION    0.1 //突然変異の確率

#define RANDOM_MAX   32767
#define BEFORE       0
#define AFTER        1

char chrom[POP_SIZE][LEN_CHROM]; //染色体
int fitness[POP_SIZE];          //適合度
int sort_chrom[POP_SIZE];       //選択された親個体の要素番号（高い順）
int max, min, sumfitness;       //適合度のmax,min,sum
int n_min;                      //適合度のminの添字
static unsigned long int next=1;    //擬似乱数

char Num2Sound[25][5] = {
    "  ",    // 0
    "ド4",    // 1 ド
    "ド#4",   // 2
    "レ4",    // 3 レ
    "レ#4",   // 4
    "ミ4",    // 5 ミ
    "ファ4",    // 6 ファ
    "ファ#4",   // 7
    "ソ4",    // 8 ソ
    "ソ#4",   // 9
    "ラ4",    // 10 ラ
    "ラ#4",   // 11
    "シ4",    // 12 シ
    "ド5",    // 13 ド 523Hz
    "ド5#",   // 14
    "レ5",    // 15 レ  587Hz
    "レ#5",   // 16
    "ミ5",    // 17 ミ 659Hz
    "ファ5",    // 18   698Hz
    "ファ#5",   // 19
    "ソ5",    // 20   784Hz
    "ソ#5",   // 21
    "ラ5",    // 22   880Hz
    "ラ#5",   // 23
    "シ5"     // 24   988Hz
};

char Num2Bbdur[25][5] = {
    1, //
    3, // ド
    5, //
    6, // レ
    8, //
    10, // ミ
    12, // ファ
    13, //
    15, // ソ
    17, //
    18, // ラ
    20, //
    22, // シ
    24, // ド 523Hz
    1, //
    3, // レ  587Hz
    5, //
    6, // ミ 659Hz
    8, //   698Hz
    10, //
    12, //   784Hz
    13, //
    15, //   880Hz
    17, //
    18, //   988Hz
}
//ファイルオープンエラー
void FileOpenError(FILE *fp){
    if (fp == NULL){
        printf("ファイルが開けませんでした。\n");
        exit(1);
    }
}

//引数iに対応する個体の遺伝子情報と適合度を表示
void PrintEachChromFitness(int i){
    int j;
    FILE *fp;
    
    fp = fopen("result_file.txt", "a");
    FileOpenError(fp);
    
    printf("個体%d : %d\n", i, fitness[i]);
    for(j=0; j<LEN_CHROM; j++){
        if((j%32 == 0) && (j != 0))  printf("\n");  //セルが32個毎に改行
        printf("%2s ", Num2Sound[chrom[i][j]]);
    }
    printf("\n");
    
    //ファイルへの書き込み
    fprintf(fp, "個体%d : %d\n", i, fitness[i]);
    for(j=0; j<LEN_CHROM; j++){
        if((j%32 == 0) && (j != 0))  fprintf(fp, "\n");
        fprintf(fp, "%2s ", Num2Sound[chrom[i][j]]);
    }
    fprintf(fp, "\n");
    fclose(fp);

    //ファイルへの書き込み（play sound 用）
    fp = fopen("sounds.txt", "w");
    FileOpenError(fp);

    for(j=0; j<LEN_CHROM/3; j++){
        fprintf(fp, "%d %d %d\n", chrom[i][j], chrom[i][j+32], chrom[i][j+64]);
    }
    fprintf(fp, "0 0 0\n");
    fprintf(fp, "0 0 0\n");
    fclose(fp);
}

//個体ごとに遺伝子情報と適合度を表示する関数を呼び出す
void PrintChromFitness(){
    int i;
    for(i=0; i<POP_SIZE; i++){
        PrintEachChromFitness(i);
    }
}

//世代数，適合度の最大値，最小値，合計値，平均値を表示
void PrintStatistics(int gen){
    double ave;
    FILE *fp;
    
    fp = fopen("result_file.txt", "a");
    FileOpenError(fp);
    
    ave = (double)sumfitness/(double)POP_SIZE;
    printf("[gen=%2d] max=%d min=%d sumfitness=%d ave=%f\n", gen, max, min, sumfitness, ave);
    
    //ファイルへの書き込み
    fprintf(fp, "[gen=%2d] max=%d min=%d sumfitness=%d ave=%f\n", gen, max, min, sumfitness, ave);
    fclose(fp);
}

//選択された親，除去された個体，新たな子供のそれぞれの遺伝子情報と適合度を表示する関数を呼び出す
void PrintCrossover(int flag, int parent1, int parent2, int child1, int child2, int n_cross[]){
    int i;
    FILE *fp;
    
    fp = fopen("result_file.txt", "a");
    FileOpenError(fp);
    
    switch(flag){
        case BEFORE:
            printf("<parent1> ");
            fprintf(fp, "<parent1> ");  //ファイルへの書き込み
            PrintEachChromFitness(parent1);
            printf("<parent2> ");
            fprintf(fp, "<parent2> ");  //ファイルへの書き込み
            PrintEachChromFitness(parent2);
            printf("<delete1> ");
            fprintf(fp, "<delete1> ");  //ファイルへの書き込み
            PrintEachChromFitness(child1);
            printf("<delete2> ");
            fprintf(fp, "<delete2> ");  //ファイルへの書き込み
            PrintEachChromFitness(child2);
            break;
        case AFTER:
            //交叉位置を表示
            printf("交叉位置:\n");
            for(i=0; i<LEN_CHROM; i++){
                if((i%32 == 0) && (i != 0))  printf("\n");  //セルが32個毎に改行
                printf("%2d ", n_cross[i]);
            }
            
            //ファイルへの書き込み
            fprintf(fp, "交叉位置:\n");
            for(i=0; i<LEN_CHROM; i++){
                if((i%32 == 0) && (i != 0))  fprintf(fp, "\n");
                fprintf(fp, "%2d ", n_cross[i]);
            }            

            printf("\n");
            fprintf(fp, "\n");
            
            printf("<child1> ");
            fprintf(fp, "<child1> ");   //ファイルへの書き込み
            PrintEachChromFitness(child1);
            printf("<child2> ");
            fprintf(fp, "<child2> ");   //ファイルへの書き込み
            PrintEachChromFitness(child2);
            printf("-----------------------------------------------------------------\n");
            fprintf(fp, "-----------------------------------------------------------------\n");
            break;
    }
    fclose(fp);
}

//突然変異位置と突然変異前後の子供個体の表示
void PrintMutation(int flag, int child, int n_mutate){
    FILE *fp;
    
    fp = fopen("result_file.txt", "a");
    FileOpenError(fp);
    
    switch(flag){
        case BEFORE:
            printf("<child(OLD)> ");
            fprintf(fp, "<child(OLD)> ");   //ファイルへの書き込み
            PrintEachChromFitness(child);
            printf("n_mutate=%d\n", n_mutate);  //突然変異位置の表示
            fprintf(fp, "n_mutate=%d\n", n_mutate); //ファイルへの書き込み
            break;
        case AFTER:
            printf("<child(NEW)> ");
            fprintf(fp, "<child(NEW)> ");   //ファイルへの書き込み
            PrintEachChromFitness(child);
            printf("-----------------------------------------------------------------\n");
            fprintf(fp, "-----------------------------------------------------------------\n");
            break;
    }
    fclose(fp);
}

// 擬似乱数
int Rand(void){
    next = next*1103515245+12345;
    return (unsigned int)(next/65536)%32768;
}

void Srand(unsigned int seed){
    next = seed;
}

// 癒しの周波数に近い音が使われているか
int ScoreHz(int sound1, int sound2, int sound3){
    // 3つのうちどれか一つでもドに近ければ return 1
    int S_MIN = 12 - 5;
    int S_MAX = 12 + 5;
    if(S_MIN < sound1 && sound1 < S_MAX) return 1;
    else if(S_MIN < sound2 && sound2 < S_MAX) return 1;
    else if(S_MIN < sound3 && sound3 < S_MAX) return 1;
    return 0;
}

// 短い音が一番多いか（最大3を返す）
int ScoreRhythm(int i){
    int j;
    int sum=0;
    int n_quarter=0, n_half=0, n_whole=0; //４分音符，２分音符，全音符の個数
    int score=0;
    for(j=1;j<LEN_CHROM/3;j++){
        if (chrom[i][j-1] == chrom[i][j]) sum++;
        else {
            if(sum == 1) n_quarter += 1;
            else if(sum == 2) n_half += 1;
            else if(sum == 4) n_whole += 1;
            sum = 0;
        }
    } 
    if(n_quarter > n_half) score += 1;
    if(n_half > n_whole) score += 1;
    if(n_quarter > n_whole) score += 1;
    return score;
}

//不協和音なら0を返す
int ScoreChord(int sound1, int sound2, int sound3){
    int diff12, diff23, diff31;
    diff12 = abs(sound1 - sound2);
    diff23 = abs(sound2 - sound3);
    diff31 = abs(sound3 - sound1);
    if((diff12!= 4) && (diff12 != 5) && (diff12 != 7)) return 0; //長3度, 完全4度, 完全5度  
    if((diff23 != 4) && (diff23 != 5) && (diff23 != 7)) return 0;
    if((diff31 != 4) && (diff31 != 5) && (diff31 != 7)) return 0;
    return 1;
}

// 二分探索：配列aのなかにtargetがあれば1を返す
int BinarySearch(int a[], int target, int ARRAY_SIZE){
    int left = 0;
    int right = ARRAY_SIZE;
    int mid;

    while(left <= right) {
        mid = (left + right) / 2; /* calc of middle key */
        if (a[mid] == target) {
            return 1;
        } else if (a[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return 0;
}

//Edurの音があれば+1（最大 3を返す）
int ScoreEdur(int sound1, int sound2, int sound3){
    int ARRAY_SIZE = 14;
    int Edur[14] = {1, 3, 4, 6, 8, 9, 11, 13, 15, 16, 18, 20, 21, 23};
    int score = 0;
    score += BinarySearch(Edur, sound1, ARRAY_SIZE);
    score += BinarySearch(Edur, sound2, ARRAY_SIZE);
    score += BinarySearch(Edur, sound3, ARRAY_SIZE);

    return score;
}

// 目的関数
int ObjFunc(int i){
    int j, sound1, sound2, sound3;
    int score_hz=0;
    int score_rhythm=0;
    int score_chord=0;
    int score_interval=0;
    int score_dur=0;

    for(j=0;j<LEN_CHROM/3;j++){
        sound1 = chrom[i][j];
        sound2 = chrom[i][j+32];
        sound3 = chrom[i][j+64];
        // 癒し周波数かどうか：真ん中のド(音番号 12)に近ければ加点
        score_hz += ScoreHz(sound1, sound2, sound3);

        // 不協和音が無ければ+1
        score_chord += ScoreChord(sound1, sound2, sound3);

        // 飛躍が少ないか
        if(j!=0){
            if(abs(chrom[i][j]-chrom[i][j]) < 8) score_interval += 1;
        }
        
        // 同じ調を使っているか（Ebで固定）
        // score_dur += ScoreEdur(sound1, sound2, sound3);
        
    }
    // 短い音が一番多いか（リズム）
    score_rhythm = ScoreRhythm(i) * 4; //*2

    return score_hz + score_rhythm + score_chord + score_interval + score_dur;
}

//初期化
void Initialize(){
    int i, j;
    for(i=0;i<POP_SIZE;i++){
        for(j=0;j<LEN_CHROM;j++){
            chrom[i][j]=Rand()%LEN_SOUND;
            chrom[i][j]=Num2Bbdur[chrom[i][j]];
        }
        fitness[i]=ObjFunc(i);
    }

    printf("First Population\n");
    PrintChromFitness();
    printf("------------------\n");
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

//突然変異
void Mutation(int child){
    int n_mutate;
    int scale;
    double random;

    random = (double)rand()/(double)RAND_MAX; // 0<=random<1
    if((int)rand < (int)(P_MUTATION*100)){  //確率5%未満なら突然変異
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

void Statistics(){
    int i;
    max = 0;
    min = POP_SIZE;
    sumfitness=0;

    for(i=0;i<POP_SIZE;i++){
        if(fitness[i]>max) max=fitness[i];
        else if(fitness[i]<max){
            min = fitness[i];
            n_min = i;
        }
        sumfitness += fitness[i];
    }
}

//第1世代の処理
void Generation(int gen){
    int parent1, parent2; //親の要素数を格納
    int child1, child2;   //子供の要素数を格納
    int n_gen = 3;
    int n_delete = 7;     //子供に置き換える除去する個体の要素数を格納
    int i, j;

    //世代交替（？）
    PrintStatistics(gen);
    Statistics();
    Select();   //どっちかというとソート
    for(i=0; i<(n_gen-1); i++){     //(1,2),(1,3),(2,3)の順で子供*2を作る
       for(j=i+1; j<n_gen; j++) 
          parent1 = sort_chrom[i];  //適応度の高い個体の要素順に並べたsort_chromから取り出す
          parent2 = sort_chrom[j];  //上記に同じ
          child1 = n_delete;        //適応度が7～12番目の個体を新しい子供で置き換えていく
          child2 = n_delete+1;      //上記に同じ
          Crossover(parent1, parent2, child1, child2);
          Mutation(child1);
          Mutation(child2);

          n_delete = n_delete + 2;
    }
}

//メイン関数
int main(int argc, char**argv)
{
    int gen;
    
    Initialize();
    for(gen=1; gen<=MAX_GEN; gen++){
        Generation(gen);
    }
    return 0;
}
