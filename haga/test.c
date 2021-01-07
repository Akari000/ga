/*------------------------------------------------------
・基本的なプログラムは担当者が作成
・それぞれの担当プログラムの合体＆エラー処理　⇒　芳賀あかり
・エラーなし実行結果の不具合の原因究明＆修正　⇒　息優奈・芳賀あかり
--------------------------------------------------------*/

//担当：息優奈・芳賀あかり
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_GEN      30  //最大世代交替
#define POP_SIZE     12  //集団のサイズ
#define LEN_CHROM    32*3//一個体の遺伝子の長さ
#define LEN_SOUND    24+1 //音数(無音は0)
#define GEN_GAP      0.2 //世代交替の割合
// #define P_CROSS      0.5 //一様交叉率
#define P_MUTATION    0.01 //突然変異の確率

#define RANDOM_MAX   32767
#define BEFORE       0
#define AFTER        1

char chrom[POP_SIZE][LEN_CHROM]; //染色体
int fitness[POP_SIZE];          //適合度
int sort_chrom[POP_SIZE];       //選択された親個体の要素番号（高い順）
int max, min, sumfitness;       //適合度のmax,min,sum
int n_min;                      //適合度のminの添字
double P_CROSS=0.5;             //一様交叉率
int Edur[14] = {1, 3, 4, 6, 8, 9, 11, 13, 15, 16, 18, 20, 21, 23}; //Edurの音リスト
int Cdur[14] = {1, 3, 4, 6, 8, 10, 12, 13, 15, 17, 18, 20, 22, 24}; //Edurの音リスト
static unsigned long int next=1;    //擬似乱数

//担当：芳賀あかり
char Num2Sound[25][10] = {
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

//担当：芳賀あかり
int Num2Cdur[25] = {
    0,
    1, //
    3, //
    5, //
    6, //
    8, //
    10, //
    12, //
    13, //
    15, //
    17, //
    18, //
    20, //
    22, //
    24, //
    0,
    1, //
    3, //
    5, //
    6, //
    8, //
    10, //
    12, //
    13, //
    15, //
};

//担当：息優奈
//ファイルオープンエラー
void FileOpenError(FILE *fp){
    if (fp == NULL){
        printf("ファイルが開けませんでした。\n");
        exit(1);
    }
}

//担当：息優奈・芳賀あかり
//play sound用のファイル書き込み
void PrintSoundFile(){
    int j;
    FILE *fp;

    fp = fopen("sounds.txt", "a");
    FileOpenError(fp);

    for(j=0; j<LEN_CHROM/3; j++){
        fprintf(fp, "%d %d %d\n", chrom[0][j], chrom[0][j+32], chrom[0][j+64]);
    }
    fprintf(fp, "0 0 0\n");
    fprintf(fp, "0 0 0\n");
    fprintf(fp, "0 0 0\n");
    fprintf(fp, "0 0 0\n");
    fclose(fp);
}

//担当：息優奈、play soud用ファイル出力追加：芳賀あかり
//引数iに対応する個体の遺伝子情報と適合度を表示
void PrintEachChromFitness(int i, char *label){
    int j;
    FILE *fp;
    
    fp = fopen("../_data/result_file.txt", "w");
    FileOpenError(fp);
    
    printf("個体%d : %d\n", i, fitness[i]);
    for(j=0; j<LEN_CHROM; j++){
        if((j%32 == 0) && (j != 0))  printf("\n");  //セルが32個毎に改行
        printf("%s\t", Num2Sound[chrom[i][j]]);
    }
    printf("\n");
    
    //ファイルへの書き込み
    fprintf(fp, "%s", label);
    fprintf(fp, "個体%d : %d\n", i, fitness[i]);
    for(j=0; j<LEN_CHROM; j++){
        if((j%32 == 0) && (j != 0))  fprintf(fp, "\n");
        fprintf(fp, "%s\t", Num2Sound[chrom[i][j]]);
    }
    fprintf(fp, "\n");
    fclose(fp);
}

//担当：息優奈
//個体ごとに遺伝子情報と適合度を表示する関数を呼び出す
void PrintChromFitness(){
    int i;
    for(i=0; i<POP_SIZE; i++){
        PrintEachChromFitness(i, "");
    }
}

//担当：息優奈
//世代数，適合度の最大値，最小値，合計値，平均値を表示
void PrintStatistics(int gen){
    double ave;
    FILE *fp;
    
    fp = fopen("../_data/result_file.txt", "a");
    FileOpenError(fp);
    
    ave = (double)sumfitness/(double)POP_SIZE;
    printf("[gen=%2d] max=%d min=%d sumfitness=%d ave=%f\n", gen, max, min, sumfitness, ave);
    
    //ファイルへの書き込み
    fprintf(fp, "[gen=%2d] max=%d min=%d sumfitness=%d ave=%f\n", gen, max, min, sumfitness, ave);
    fclose(fp);

    fp = fopen("../_data/average.txt", "a");
    FileOpenError(fp);
    fprintf(fp, "%f\n", ave);
    fclose(fp);
}

//担当：息優奈
//選択された親，除去された個体，新たな子供のそれぞれの遺伝子情報と適合度を表示する関数を呼び出す
void PrintCrossover(int flag, int parent1, int parent2, int child1, int child2, int n_cross[]){
    int i;
    FILE *fp;
    
    fp = fopen("../_data/result_file.txt", "a");
    FileOpenError(fp);
    
    switch(flag){
        case BEFORE:
            printf("<parent1> ");
            // fprintf(fp, "<parent1> ");  //ファイルへの書き込み
            PrintEachChromFitness(parent1, "<parent1> \n");
            printf("<parent2> ");
            // fprintf(fp, "<parent2> ");  //ファイルへの書き込み
            PrintEachChromFitness(parent2, "<parent2> \n");
            printf("<delete1> ");
            // fprintf(fp, "<delete1> ");  //ファイルへの書き込み
            PrintEachChromFitness(child1, "<delete1> \n");
            printf("<delete2> ");
            // fprintf(fp, "<delete2> ");  //ファイルへの書き込み
            PrintEachChromFitness(child2, "<delete2> \n");
            break;
        case AFTER:
            //交叉位置を表示
            printf("交叉率：%lf，交叉位置:\n", P_CROSS);
            for(i=0; i<LEN_CHROM; i++){
                if((i%32 == 0) && (i != 0))  printf("\n");  //セルが32個毎に改行
                printf("%2d ", n_cross[i]);
            }
            
            //ファイルへの書き込み
            fprintf(fp, "交叉率：%lf，交叉位置:\n", P_CROSS);
            for(i=0; i<LEN_CHROM; i++){
                if((i%32 == 0) && (i != 0))  fprintf(fp, "\n");
                fprintf(fp, "%2d ", n_cross[i]);
            }            

            printf("\n");
            fprintf(fp, "\n");
            
            printf("<child1> ");
            // fprintf(fp, "<child1> ");   //ファイルへの書き込み
            PrintEachChromFitness(child1, "<child1> ");
            printf("<child2> ");
            // fprintf(fp, "<child2> ");   //ファイルへの書き込み
            PrintEachChromFitness(child2, "<child2> ");
            printf("-----------------------------------------------------------------\n");
            fprintf(fp, "-----------------------------------------------------------------\n");
            break;
    }
    fclose(fp);
}

//担当：息優奈
//突然変異位置と突然変異前後の子供個体の表示
void PrintMutation(int flag, int child, int n_mutate){
    FILE *fp;
    
    fp = fopen("../_data/result_file.txt", "a");
    FileOpenError(fp);
    
    switch(flag){
        case BEFORE:
            printf("<child(OLD)> ");
            // fprintf(fp, "<child(OLD)> ");   //ファイルへの書き込み
            PrintEachChromFitness(child, "<child(OLD)> ");
            printf("n_mutate=%d\n", n_mutate);  //突然変異位置の表示
            fprintf(fp, "n_mutate=%d\n", n_mutate); //ファイルへの書き込み
            break;
        case AFTER:
            printf("<child(NEW)> ");
            // fprintf(fp, "<child(NEW)> ");   //ファイルへの書き込み
            PrintEachChromFitness(child, "<child(NEW)> ");
            printf("-----------------------------------------------------------------\n");
            fprintf(fp, "-----------------------------------------------------------------\n");
            break;
    }
    fclose(fp);
}

//担当：芳賀あかり
// 擬似乱数
int Rand(void){
    next = next*1103515245+12345;
    return (unsigned int)(next/65536)%32768;
}

void Srand(unsigned int seed){
    next = seed;
}

//担当：芳賀あかり
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

//担当：芳賀あかり
// 短い音が一番多いか（最大3を返す）
int ScoreRhythm(int i){
    int j;
    int tail=32-1; // 最後尾のindex
    int sum=0; //root音が連続した回数
    int sum_all=0; //3つ全ての音が連続した回数
    int convergent=0; //収束するか (最初と最後の音が一致すれば加点)
    int n_eighth=0, n_quarter=0, n_half=0, n_whole=0; //４分音符，２分音符，全音符の個数

    for(j=0;j<(LEN_CHROM/3)-1;j++){
        if (chrom[i][j] == chrom[i][j+1]){
            sum++;
            if (chrom[i][j+32] != chrom[i][j+32+1] && chrom[i][j+64] != chrom[i][j+64+1]) sum_all++;
            else if (chrom[i][j+32] != chrom[i][j+64+1] && chrom[i][j+64] != chrom[i][j+32+1]) sum_all++;
        }
        else {
            // 最後ならさらに加点
            if(j==tail){
                if(sum == 1) n_eighth += 2;
                else if(sum == 2) n_quarter += 2;
                else if(sum == 4) n_half += 2;
                else if(sum == 8) n_whole += 2;
                sum = 0;
                continue;
            }
            if(sum == 1) n_eighth += 1;
            else if(sum == 2) n_quarter += 1;
            else if(sum == 4) n_half += 1;
            else if(sum == 8) n_whole += 1;
            sum = 0;
        }
    }
    // 最初と最後の音が同じならさらに加点
    if (chrom[i][j] == chrom[i][tail]){
        convergent++;
        if (chrom[i][32] != chrom[i][tail+32] && chrom[i][64] != chrom[i][tail+64]) convergent++;
        else if (chrom[i][32] != chrom[i][tail+64] && chrom[i][64] != chrom[i][tail+32]) convergent++;
    }

    return (
        (n_quarter) +
        (n_half) +
        sum_all +
        convergent
    );
}

//担当：芳賀あかり
//協和音なら1を返す
int ScoreChord(int root, int sound1, int sound2){
    int diff1, diff2;
    //root音 からの差をそれぞれ見る
    diff1 = abs(root - sound1);
    diff2 = abs(root - sound2);
    if(sound1 != 0){
        if((diff1 != 4) && (diff1 != 5) && (diff1 != 7) && (diff1 != 12)) return 0;
    }
    if(sound2 != 0){
        if((diff2 != 4) && (diff2 != 5) && (diff2 != 7) && (diff2 != 12)) return 0;
    }
    return 1;
}

//担当：芳賀あかり
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

//担当：芳賀あかり
//Edurの音があれば+1（最大 3を返す）
int ScoreEdur(int sound){
    int ARRAY_SIZE = 14;
    int score = 0;
    score += BinarySearch(Cdur, sound, ARRAY_SIZE);

    return score;
}

//担当：芳賀あかり
//音の跳躍が閾値以下の幅なら1を返す
int ScoreInterval(int sound1, int sound2){
    if((sound1-sound2) < 5) return 1;
    return 0;
}

//担当：芳賀あかり
// 単音 > 二和音 > 3和音の順で音多いと加点する
int ScoreNChord(int sound1, int sound2){
    int sum=0;
    int score;
    if(sound1==0) sum++;
    if(sound2==0) sum++;
    if (sum == 0) score = 0;
    if (sum == 1) score = 1;
    if (sum == 2) score = 2;
    return score;
}

//担当：芳賀あかり
// 目的関数
int ObjFunc(int i){
    int j, root, sound1, sound2;
    int score_hz=0;         // 0~32
    int score_rhythm=0;     // 0~32
    int score_chord=0;      // 0~32
    int score_interval=0;   // 0~32
    int score_dur=0;        // 0~32
    int score_n_chord=0;    // 0~32

    for(j=0;j<LEN_CHROM/3;j++){
        root = chrom[i][j];
        sound1 = chrom[i][j+32];
        sound2 = chrom[i][j+64];

        // 癒し周波数かどうか：真ん中のド(音番号 12)に近ければ加点
        // score_hz += ScoreHz(root, sound1, sound2);

        // 不協和音が無ければ+1
        score_chord += ScoreChord(root, sound1, sound2);

        // 飛躍が少ないか
        if(j!=0){
            // root音のみ見る
            score_interval += ScoreInterval(chrom[i][j], chrom[i][j-1]);
            score_interval += ScoreInterval(chrom[i][j+32], chrom[i][j-1]);
            score_interval += ScoreInterval(chrom[i][j+64], chrom[i][j-1]);
            score_interval += ScoreInterval(chrom[i][j], chrom[i][j+32-1]);
            score_interval += ScoreInterval(chrom[i][j+32], chrom[i][j+32-1]);
            score_interval += ScoreInterval(chrom[i][j+64], chrom[i][j+32-1]);
            score_interval += ScoreInterval(chrom[i][j], chrom[i][j+64-1]);
            score_interval += ScoreInterval(chrom[i][j+32], chrom[i][j+64-1]);
            score_interval += ScoreInterval(chrom[i][j+64], chrom[i][j+64-1]);
            
        }
        
        // 単音が一番多いか
        // root音は見ない．
        score_n_chord += ScoreNChord(sound1, sound2);

        // 同じ調を使っているか（Cdurで固定）
        // score_dur += ScoreEdur(root);
    }

    // 長い音があれば加点（リズム）
    score_rhythm = ScoreRhythm(i)*32;
    printf("====================objfunc======================\n");
    printf("score_hz: %d, score_rhythm: %d, score_chord*30: %d, score_interval*3: %d, score_n_chord*20: %d\n", 
            score_hz, score_rhythm, score_chord*25, score_interval*2, score_n_chord*10);
    return score_hz + score_rhythm + score_chord*25 + score_interval*2 + score_n_chord*10;
}

//担当：芳賀あかり
//初期化
void Initialize(){
    int i, j;
    for(i=0;i<POP_SIZE;i++){
        for(j=0;j<LEN_CHROM;j++){
            chrom[i][j]=rand()%LEN_SOUND;
            chrom[i][j]=Num2Cdur[chrom[i][j]];
        }
        fitness[i]=ObjFunc(i);
    }

    printf("First Population\n");
    PrintChromFitness();
    printf("------------------\n");
    
    fitness[i]=ObjFunc(i);
}

//担当：息優奈
//選択（選択の変更点memoのファイルを同じとこに置いてるよ）
void Select(){
    int i, j;
    int n_max;  //適応度が最大の個体の要素を格納
    int temp, temp_fitness[POP_SIZE];
    
    memcpy(temp_fitness, fitness, sizeof(int) * POP_SIZE);
    
    for(i=0; i<POP_SIZE; i++){
        sort_chrom[i] = i;
    }

    //個体のソート，適応度の高い順（選択ソート）
    for(i=0; i<(POP_SIZE-1); i++){
        for(j=0; j<(POP_SIZE-1-i); j++){
            if(temp_fitness[j]<temp_fitness[j+1]){ //適応度が最大の個体の要素数を探索
                temp = temp_fitness[j];
                temp_fitness[j] = temp_fitness[j+1];
                temp_fitness[j+1] = temp;
                
                temp = sort_chrom[j];
                sort_chrom[j] = sort_chrom[j+1];
                sort_chrom[j+1] = temp;
            }
        }
    }
    printf("======select==================================\n");
    for(i=0; i<(POP_SIZE-1); i++){
        printf("%d ", sort_chrom[i]);
    }
    printf("======select==================================\n");

}

//担当：息優奈
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

//担当：息優奈
//突然変異
void Mutation(int child){
    int n_mutate;
    int scale;
    double random;

    random = (double)rand()/(double)RAND_MAX; // 0<=random<1
    if((double)random < (double)(P_MUTATION)){  //確率%1未満なら突然変異
        //突然変異位置（n_mutate=0,...,96）
        n_mutate = rand()%LEN_CHROM;

        //突然変異
        PrintMutation(BEFORE, child, n_mutate);
        scale = rand() % 25;
        if(scale == chrom[child][n_mutate]){     //もしランダムに選ばれた値が元の値と同じなら次の値にする
            scale += 1;
            if(scale > 24)  //もしNum2Cdurの要素数を超える数値になったら0にする
                scale = 0;
        }
        chrom[child][n_mutate] = Num2Cdur[scale];
        fitness[child] = ObjFunc(child);
        PrintMutation(AFTER, child, n_mutate);
    }
}

//担当：芳賀あかり
void Statistics(){
    int i;
    max = 0;
    min = 1000;
    sumfitness=0;

    for(i=0;i<POP_SIZE;i++){
        if(fitness[i]>max) max=fitness[i];
        else if(fitness[i]<min){
            min = fitness[i];
            n_min = i;
        }
        sumfitness += fitness[i];
    }
}

//担当：息優奈
//第1世代の処理
void Generation(int gen){
    int parent1, parent2; //親の要素数を格納
    int child1, child2;   //子供の要素数を格納
    int n_gen = 3;
    int n_delete = 6;     //子供に置き換える除去する個体の要素数を格納
    int i, j;

    //世代交替（？）
    Statistics();
    PrintStatistics(gen);
    Select();   //どっちかというとソート
    for(i=0; i<(n_gen-1); i++){     //(1,2),(1,3),(2,3)の順で子供*2を作る
       for(j=i+1; j<n_gen; j++){
          parent1 = sort_chrom[i];  //適応度の高い個体の要素順に並べたsort_chromから取り出す
          parent2 = sort_chrom[j];  //上記に同じ
          child1 = sort_chrom[n_delete];        //適応度が7～12番目の個体を新しい子供で置き換えていく
          child2 = sort_chrom[n_delete+1];      //上記に同じ
          Crossover(parent1, parent2, child1, child2);
          if(P_CROSS > 0.01){
            Mutation(child1);
            Mutation(child2);
          }
          
          n_delete = n_delete + 2;
       }
    }
    // 同じ音があれば片方を無音にする
    for(i=0; i<POP_SIZE; i++){ 
       for(j=0; j<LEN_CHROM/3; j++){
           if (chrom[i][j] == chrom[i][j+32]) chrom[i][j+32] = 0;
           if (chrom[i][j] == chrom[i][j+64]) chrom[i][j+64] = 0;
           if (chrom[i][j+32] == chrom[i][j+64]) chrom[i][j+64] = 0;
       }
    }
    P_CROSS = P_CROSS * exp((double)-gen/10.0);
    if(P_CROSS < 0.01)  P_CROSS = 0;     //交叉率が0.01を下回ったら交叉率を0に
    
    //最終的に生成された音をサウンド用ファイルに書き込む
    if(gen == MAX_GEN){
        PrintSoundFile();
    }
}

//担当：息優奈・芳賀あかり
//メイン関数
int main(int argc, char**argv)
{
    int gen;
    FILE *fp;

    //出力ファイルの初期化
    fp = fopen("../_data/result_file.txt", "w");
    FileOpenError(fp);
    fclose(fp);

    fp = fopen("../_data/average.txt", "w");
    FileOpenError(fp);
    fclose(fp);

    fp = fopen("../_data/sounds.txt", "w");
    FileOpenError(fp);
    fclose(fp);

    srand((int)time(NULL));
    Initialize();
    for(gen=1; gen<=MAX_GEN; gen++){
        Generation(gen);
    }
    return 0;
}
