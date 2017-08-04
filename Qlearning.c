/**************************************************************************
   Q学習
**************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
// 状態
#define START 0 // 入口 (スタート)
#define WOOD  1 // 森　
#define LAKE  2 // 湖
#define POND  3 // 池
#define FIELD 4 // 草原
#define GOAL  5 // 宝 (ゴール)

// 行動
#define EAST  0 // 東 　
#define WEST  1 // 西
#define SOUTH 2 // 南　
#define NORTH 4 // 北

#define TrialNo 50 // 試行回数
#define StateNum 6  // 状態数 
#define ActNum 4    // 行動数

#define Alpha 0.1 // 学習率 　　
#define Gamma 0.9 // 減衰率　　

#define Reward 10 // 報酬

/*********************************************************************
   乱数のシードの生成
**********************************************************************/
void init_rnd()
{
  srand((unsigned int)time(NULL));
}

/********************************************************************
  乱数の発生 (0〜1の乱数)
********************************************************************/
double Random()
{
  return((double)rand()/RAND_MAX);
}

/********************************************************************
  ルールの価値の初期化
    Qvalue[s][a] : 状態sにおいて行動aを取ることの価値
********************************************************************/
void init_Q(double Qvalue[StateNum][ActNum])
{
  int s,a;

  /* すべての状態と行動の価値を0にする */
  for(s = 0; s < StateNum; s++) {
    for(a = 0; a < ActNum; a++) {
      Qvalue[s][a] = 0;
    }
  }
}

/********************************************************************
  行動の選択 (ボルツマン選択)
    state        : 状態
    Qvalue[s][a] : 状態sにおいて行動aを取ることの価値　　
    env[s][a]    : 状態sにおいて行動aを取ったときに遷移する状態
    t            : 試行回数
********************************************************************/
int select_action(int state,
		  double Qvalue[StateNum][ActNum], 
		  int env[StateNum-1][ActNum],int t)
{
  double sum;    // ルールの価値の合計
  int a;         // 行動
  int l_a;    
  double r;      // 乱数 (0～sumの間の乱数)
  double border; // 境界線
  double T=10;   // 温度パラメータ

  /* 温度パラメータを設定 */
  T=T-t;
  if(T<=1) T=1;

  /* ルールの価値の合計を計算 　　
     その状態で取れない行動(env[state][a]=-1)
     の価値は合計には含まない */
  for(a = 0; a < ActNum; a++) {
    if(env[state][a] != -1) {
      sum += exp(Qvalue[state][a] / T); // 一つ一つの方向の価値
    }
  }
  /* 0～sumの乱数を生成 */
  r = Random()*sum;
  border=0;
  for(a = 0;a < ActNum; a++){
    /* 取ることのできる行動の中から行動を選択 */
    if(env[state][a]!=-1){
      border += exp(Qvalue[state][a]/T);
    }
    /* 選択された行動を返す */
    if(r<=border){
      return a;
    }
  }
}

/********************************************************************
  ルールの価値の更新
    Qvalue[s][a] : 状態sにおいて行動aを取ることの価値
　　p_state      : 直前の状態
    act          : 行動
　　state        : 状態 (行動後の状態)　 　　
    r            : 報酬
********************************************************************/
void update_Q(double Qvalue[StateNum][ActNum], 
	      int p_state, int act, int state, int r)
{
  int a;
  double max=0; // 行動後の状態から取ることのできる行動の中での価値の最大値　

  /* 取ることのできる行動に関する価値の中で最大値を求める */ 
  for(int a = 0; a < ActNum; a++) {
    if(max < Qvalue[state][a]) { // 現在の最大値より上だったら
      max = Qvalue[state][a];    // 更新
    }
  }

  /* 状態p_stateにおいて行動actをとることの価値を更新 */
  Qvalue[p_state][act] += Alpha * (r + Gamma * max - Qvalue[p_state][act]);
}

/**************************************************************************
  メインプログラム
**************************************************************************/
int main()
{
  int t = 0;
  int s,a;
  int act;     // 行動 　
  int p_state; // 直前の状態　
  int state;   // 状態
  double Qvalue[StateNum][ActNum]; //　ルールの価値
  // 環境 (状態の遷移) 　　　　　東  　西  　南　  北
  int env[StateNum-1][ActNum]={{WOOD, -1,    POND, -1},    // 入口　　　　
			       {LAKE, START, FIELD,-1},    // 森　
			       {-1,   WOOD,  GOAL, -1},    // 湖　
			       {FIELD,-1,    -1,   START}, // 池
			       {-1    ,POND, -1,   WOOD}}; // 草原　
  int count; // エピソードの長さ　
  char *states[StateNum]={"入口","森","湖","池","草原","宝"}; // 状態(表示用)　
  char *acts[ActNum]={"東","西","南","北"};  // 行動(表示用)
  FILE *fp; // ファイルポインタ
  /* 結果保存用のファイル(result.dat)をオープン */
  if((fp=fopen("result.dat","w"))==NULL){
    printf("main() : Cannot open \"result.dat\"\n");
    exit(1);
  }

  init_rnd();      /* 乱数の初期化 */
  init_Q(Qvalue);  /* ルールの価値の初期化 */

  /* TrialNo回の試行を繰り返す */
  for(t=0;t<TrialNo;t++){
    printf("[%d]",t);
    state = START; /* 状態を初期化(STARTに設定) */
    count=0; /* エピソードの長さを0で初期化 */
    /* ゴールに到達するまで繰り返す */
    while(state!=GOAL){
      act=select_action(state,Qvalue,env,t); /* 行動を選択 */
      p_state=state; /*　状態を保存 */
      state=env[p_state][act]; /* 行動することにより状態が遷移 */
      /* ゴールに到達したら報酬を取得し、ルールの価値を更新 */
      if(state==GOAL){
	      update_Q(Qvalue,p_state,act,state,Reward);
      }
      /* ルールの価値を更新(ゴール以外では報酬は0) */
      else{
	      update_Q(Qvalue,p_state,act,state,0);
      }    
      /* 状態と行動を画面に表示 */
      printf("%s==>(%s)==>",states[p_state],acts[act]);
      /* エピソードの長さを1増やす */
      count++;
    }
    /* 最終的な状態を画面に表示 */
    printf("%s\n",states[state]);
    /* 試行回数とエピソードの長さをファイルに出力 */
    fprintf(fp,"%d %d\n",t,count);
  }
  /* ファイルをクローズ */
  fclose(fp);

  /* 最終的なルールの価値保存用のファイルをオープン */ 
  if((fp=fopen("Q.dat","w"))==NULL){
    printf("main() : Cannot open \"Q.dat\"\n");
    exit(1);
  }

  /* ルールの価値をファイルに書き出す */
  fprintf(fp," 　　　　　東　 　西　　　南　　　北\n");
  for(s=0;s<StateNum;s++){
    fprintf(fp,"%s\t",states[s]);
    for(a=0;a<ActNum;a++){
      fprintf(fp,"%6.3lf\t",Qvalue[s][a]);
    }
    fprintf(fp,"\n");
  }

  /* ファイルをクローズ  */
  fclose(fp);
}



