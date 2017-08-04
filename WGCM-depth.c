/**************************************************************************
  狼と山羊とキャベツと男 (深さ優先探索)
 **************************************************************************/
#include<stdio.h>
#include<stdlib.h>

#define MAN     0
#define WOLF    1
#define GOAT    2
#define CABBAGE 3

#define SearchMax 20

int left_side[SearchMax][4];   /* 左岸の状態 */
int right_side[SearchMax][4];  /* 右岸の状態 */

/**************************************************************************
  状態の表示 
  引数 state[i] : 左岸もしくは右岸の状態
  state[i]の配列の内容に応じて状態を表示する
  (例) state[i]={1,1,1,1}ならば [ 男 狼 山羊 キャベツ ]
  state[i]={1,0,1,0}ならば [ 男 山羊 ]
  state[i]={0,0,0,0}ならば [  ]
 **************************************************************************/
void print_state(int state[4])
{
  int i;
  printf("[");
  for(int i = 0; i < 4; i++) {
    if(state[i] == 1) { // この岸にいた場合 
      switch(i) {       // そのアイテムを出力
        case 1:
          printf("男");
          break;
        case 2:
          printf("狼");
          break;
        case 3:
          printf("山羊");
          break;
        case 4:
          printf("キャベツ");
          break;
      }
    }
  }
  printf("] ");
}

/**************************************************************************
  結果の表示 
  引数 T : ステップ数
  Tステップ目までの結果を表示する
  ステップ数: [ 左岸の状態 ] [ 右岸の状態 ]  
  (例) 0: [ 男 狼 山羊 キャベツ ] [ ] 
1: [ 狼 山羊 ] [ 男 キャベツ ] 
 **************************************************************************/
void print_ans(int T)
{
  int t;

  /* 初期状態からTステップ目までの結果を表示する 
     [ヒント] print_state()の関数が正しく作成されていれば
     print_state(left_side[t]);  で tステップ目の左岸の状態、
     print_state(right_side[t]); で tステップ目の右岸の状態
     が表示できる */

  for(t=0;t<=T;t++){
    printf("%d:", t);
    print_state(left_side[t]);  // 右岸の状態の出力
    print_state(right_side[t]); // 左岸の状態の出力
    printf("\n");
  }
}

/**************************************************************************
  状態のチェック
  引数 T                 : ステップ数
  state[i]          : チェックしたい状態
  past_state[t][i]  : 過去の状態(tステップ目の状態)
  ・狼と山羊、山羊とキャベツを残した状態でもなく、既に探索された状態
  でもなければ 1を返す
  ・それ以外は 0を返す
 **************************************************************************/
int check_state(int T,int state[4], int past_state[SearchMax][4]){
  int i,t;
  int count;

  /* 狼と山羊 もしくは 山羊とキャベツが一緒にないかをチェック
     あれば0を返す */
  if(state[WOLF] == 1 && state[GOAT] == 1) { // 狼と山羊のチェック
    return 0;
  }
  if(state[GOAT] == 1 && state[CABBAGE] == 1) { // 山羊とキャベツのチェック
    return 0;
  }

  /* 過去に同じ状態がないかをチェック  あれば0を返す
     [ヒント] past_state[t][i](tステップ目の状態)と
     state[i](現在の状態)を比較してチェック */
  for(t=0;t<T;t++){
    int f = 0;
    for(int i = 0; i < 4; i++) {
      if(past_state[t][i] == state[i]) f++;
    }
    if(f == 4) return 0;
  }  
  /* いずれにも該当しなければ1を返す */
  return 1;
}

/**************************************************************************
  深さ優先探索
  引数 T                 : ステップ数
  src_side[t][i]    : 男がいる側の状態  
  dest_side[t][i]   : 男がいない側の状態
 **************************************************************************/
void search(int T, int src_side[SearchMax][4], int dest_side[SearchMax][4]){
  int i,j;

  int src_state[4];      /* 男がいる側の状態 */
  int dest_state[4];     /* 男がいない側の状態 */
  int new_src_state[4];  /* 男がいる側の次のステップの状態 */
  int new_dest_state[4]; /* 男がいない側の次のステップでの状態 */

  /* Tステップ目の状態をコピー */
  for(i=0;i<4;i++){
    src_state[i]=src_side[T][i];
    dest_state[i]=dest_side[T][i];
  }

  for(i=0;i<4;i++){ /* 0: 男 1: 狼 2: 山羊 3: キャベツ を順に調べる */
    if(src_state[i]==1){ /* 移動できるのであれば(男と同じ側にいるのであれば) */

      /* iと男を移動(iが0の場合は男のみ移動)した後の状態を 
         new_src_state[], new_dest_state[] に格納 
         [ヒント] 現在の状態 (src_state[], dest_state[])を
         new_src_state[], new_dest_state[]にコピーし,
         iと男が移動した場合に値がどのように変化するかを設定 */

      for(j = 0; j < 4; j++){
        new_src_state[j] = src_state[j];
        new_dest_state[j] = dest_state[j];
      }

      new_src_state[0] = 0;
      new_dest_state[0] = 1;
      if(i != 0) { // 男以外も移動する場合
        new_src_state[i] = 0;
        new_dest_state[i] = 1;    
      }

      /* iと男を移動(iが0の場合は男のみ移動)した後の状態が有効かどうかを 
         チェックし、有効であれば 岸の状態を更新し、次に進む */
      if(check_state(T,new_src_state,src_side)){
        /* 男が左岸にいる場合(Tが偶数の場合) 
           left_side[T+1][]に new_src_state[]をコピー 
           right_side[T+1][]に new_dest_state[]をコピー */
          if(T%2==0){
          for(j = 0; j < 4; j++) {
            left_side[T + 1][j] = new_src_state[j];
            right_side[T + 1][j] = new_dest_state[j];
          }
        }
        /* 男が右岸にいる場合(Tが奇数の場合) 
           right_side[T+1][]に new_src_state[]をコピー 
           left_side[T+1][]に new_dest_state[]をコピー */
        else{
          /*** この部分を自分で作成する ***/
          for(j = 0; j < 4; j++) {
            right_side[T + 1][j] = new_src_state[j];
            left_side[T + 1][j] = new_dest_state[j];
          }
        }
        /* 右岸にすべてが移動していれば 結果を表示して終了 */
        if(right_side[T + 1][0] == 1 && right_side[T + 1][1] == 1 && right_side[T + 1][2] == 1 && right_side[T + 1][3] == 1){
          print_ans(T+1);
          exit(0);
        }
        /* そうでなければ再帰的に探索を続ける */
        else{
          search(T+1,dest_side,src_side);
        }
      }
    }
  }
}
  /**************************************************************************
    メインプログラム
   **************************************************************************/
  int main()
  {
    int i,t;

    /* 配列の初期化 (-1を設定) */
    for(t=0;t<SearchMax;t++){
      for(i=0;i<4;i++){
        left_side[t][i]=-1;
        right_side[t][i]=-1;
      }
    }

    /* 初期状態の設定 */
    for(i=0;i<4;i++){
      left_side[0][i]=1;
      right_side[0][i]=0;
    }

    /* 探索 */
    search(0,left_side,right_side);
    return 0;
  }
