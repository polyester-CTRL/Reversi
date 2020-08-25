
#include "Game.hpp"
#define font FontAsset(U"Display")
//FontAsset(U"Score")(m_score).drawAt(Scene::Center().x, 30);
typedef struct {
	int x;
	int y;
	int status;
	int turn;
}Coordinate;

Coordinate record[64] = { 0 }; //一手ごとに記録する
int innerCell[10][10] = { 0 }; //石の場所を記憶する 領域外も含む
int deepCell[100][10][10] = { 0 }; //ｎ手先を読む用
int turn = 1; //手数を数える

int stack[60] = { 0 };
int sp = 0;

bool pleaseSkip = false;

int flip_stone(int, Coordinate, int);
void push(int);
int pop();
int staticScore(int);
int cellScore(int, int, int);



//Window::SetTitle(U"Reversi");
const Coordinate Origin = { 170, 90 };

const int32 gridSize = 60;
const Rect rectFrame(Origin.x, Origin.y, gridSize * 8, gridSize * 8);
const Line LineX(Origin.x + 60, Origin.y, Origin.x + 60, Origin.y + 480);
const Line LineY(Origin.x, Origin.y + gridSize, Origin.x + 480, Origin.y + gridSize);
Rect cell[8][8]; //クリックされた領域を検出するだけ
Circle stone[8][8]; //石の形状、大きさを決め、座標をいれて石を表示する
Circle smallStone[8][8];
//const Font font(100); //メッセージを表示するために用意

bool search = true;  //探索するかしないか決める
bool ai = true;      //コンピュータ対戦するかしないか
bool put = false;    //石が置かれた時だけtrueにする


int32 canPut[10][10] = { 0 };

int32 oncePerSecond = 0; //1秒に一回くらい実行したいものに使う

Game::Game(const InitData& init)
	: IScene(init)
{
	// 横 (Scene::Width() / blockSize.x) 個、縦 5 個のブロックを配列に追加する
	for (auto p : step(Size((Scene::Width() / blockSize.x), 5)))
	{
		m_blocks << Rect(p.x * blockSize.x, 60 + p.y * blockSize.y, blockSize);
	}

	//中心座標を代入する
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			cell[i][j] = Rect(Origin.x + gridSize * i, Origin.y + gridSize * j, 60);
			stone[i][j] = Circle(Origin.x + gridSize * i + gridSize / 2, Origin.y + gridSize * j + gridSize / 2, 25);
			smallStone[i][j] = Circle(Origin.x + gridSize * i + gridSize / 2, Origin.y + gridSize * j + gridSize / 2, 5);
		}
	}

	//壁の部分の配列は-1を入れる
	for (int i = 0; i < 10; i++)
	{
		innerCell[i][0] = -1;
		innerCell[i][9] = -1;
		innerCell[0][i] = -1;
		innerCell[9][i] = -1;
		for (int j = 0; j < DEPTH; j++) {
			deepCell[j][i][0] = -1;
			deepCell[j][i][9] = -1;
			deepCell[j][0][i] = -1;
			deepCell[j][9][i] = -1;
		}
	}
	innerCell[4][4] = deepCell[1][4][4] = 1;
	innerCell[4][5] = deepCell[1][4][5] = 2;
	innerCell[5][4] = deepCell[1][5][4] = 2;
	innerCell[5][5] = deepCell[1][5][5] = 1;
	Window::SetTitle(U"Reversi");

	
  int32 tmpTime = 0;
}

void Game::update()
{
	/****************************************************************************/
	/*
	// パドルを操作
	m_paddle = Rect(Arg::center(Cursor::Pos().x, 500), 60, 10);

	// ボールを移動
	m_ball.moveBy(m_ballVelocity * Scene::DeltaTime());

	// ブロックを順にチェック
	for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it)
	{
		// ボールとブロックが交差していたら
		if (it->intersects(m_ball))
		{
			// ボールの向きを反転する
			(it->bottom().intersects(m_ball) || it->top().intersects(m_ball) ? m_ballVelocity.y : m_ballVelocity.x) *= -1;

			// ブロックを配列から削除（イテレータが無効になるので注意）
			m_blocks.erase(it);

			// スコアを加算
			++m_score;

			// これ以上チェックしない  
			break;
		}
	}

	// 天井にぶつかったらはね返る
	if (m_ball.y < 0 && m_ballVelocity.y < 0)
	{
		m_ballVelocity.y *= -1;
	}

	if (m_ball.y > Scene::Height())
	{
		changeScene(State::Title);
		getData().highScore = Max(getData().highScore, m_score);
	}

	// 左右の壁にぶつかったらはね返る
	if ((m_ball.x < 0 && m_ballVelocity.x < 0) || (Scene::Width() < m_ball.x && m_ballVelocity.x > 0))
	{
		m_ballVelocity.x *= -1;
	}

	// パドルにあたったらはね返る
	if (m_ballVelocity.y > 0 && m_paddle.intersects(m_ball))
	{
		// パドルの中心からの距離に応じてはね返る向きを変える
		m_ballVelocity = Vec2((m_ball.x - m_paddle.center().x) * 10, -m_ballVelocity.y).setLength(speed);
	}

	*/
	/****************************************************************************/

	oncePerSecond++;
  
	if (getData().tmpTime != (int32)Scene::Time()) {
		oncePerSecond = 0;
    getData().tmpTime = (int32)Scene::Time();
	}
	//盤面
	Scene::SetBackground(ColorF(0.2,0.8,0.3));
  //Scene::SetBackground(ColorF(0.2, 0.8, 0.4));
	rectFrame.drawFrame(0, 3, Palette::Black);
	//LineX.draw(3, Palette::Black);
	//LineY.draw(3, Palette::Black);
	for (int i = 1; i < 8; i++) {
		Line(Origin.x + gridSize * i, Origin.y, Origin.x + gridSize * i, Origin.y + gridSize * 8).draw(3, Palette::Black);
		Line(Origin.x, Origin.y + gridSize * i, Origin.x + gridSize * 8, Origin.y + gridSize * i).draw(3, Palette::Black);
	}


  //石を置ける場所を見つける
  Coordinate c;

  //cellごとにスコア計算
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      ///////////元の部分
      c = { i + 1, j + 1, (turn + 1) % 2 + 1, turn }; //探索関数に送る情報
      for (int k = 0; k < 8; k++) { //探索方向を決める

        canPut[i + 1][j + 1] += flip_stone(k, c, 0); //ひっくり返さない
        if (innerCell[i + 1][j + 1] != 0) {
          canPut[i + 1][j + 1] = 0; //置ける場所でなかったら0に戻す
        }
      }
      /*
      if (canPut[i + 1][j + 1] > 0) { //ひっくり返り得る数を表示
        font(canPut[i + 1][j + 1]).draw(Origin.x + gridSize * i + 5, Origin.y + gridSize * j + 5, Palette::White);
        smallStone[i][j].draw(Palette::Darkgoldenrod);
      }
      */
      //////////終わり
    }
  }
  int32 score[10][10] = { 0 };
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      if (canPut[i][j] > 0) {
        score[i][j] = cellScore(i, j, 1);
        //FontAsset(U"cellNum")(score[i][j]).draw(Origin.x + gridSize * (i - 1) + 8, Origin.y + gridSize * (j - 1) + 8, Palette::Black);
      }
      else {
        score[i][j] = 0;
      }
    }
  }
	

	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			canPut[i][j] = 0;
		}
	} 
  
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			c = { i + 1, j + 1, (turn + 2) % 2 + 1, turn }; //探索関数に送る情報
			//if (oncePerSecond % 3 == 0)
				for (int k = 0; k < 8; k++) { //探索方向を決める

					canPut[i + 1][j + 1] += flip_stone(k, c, 0); //ひっくり返さない
					if (innerCell[i + 1][j + 1] != 0) {
						canPut[i + 1][j + 1] = 0; //置ける場所でなかったら0に戻す
					}
				}
			if (canPut[i + 1][j + 1] > 0) { //ひっくり返り得る数を表示
				//font(canPut[i + 1][j + 1]).draw(Origin.x + gridSize * i, Origin.y + gridSize * j, Palette::White);

			}
			else {
				//font(canPut[i + 1][j + 1]).draw(Origin.x + gridSize * i, Origin.y + gridSize * j, Palette::Black);
			}
		}
	}
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      canPut[i][j] = 0;
    }
  }
  
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      ///////////元の部分
      c = { i + 1, j + 1, (turn + 1) % 2 + 1, turn }; //探索関数に送る情報
      //if (oncePerSecond % 3 == 0)
      for (int k = 0; k < 8; k++) { //探索方向を決める

        canPut[i + 1][j + 1] += flip_stone(k, c, 0); //ひっくり返さない
        if (innerCell[i + 1][j + 1] != 0) {
          canPut[i + 1][j + 1] = 0; //置ける場所でなかったら0に戻す
        }
      }
      if (canPut[i + 1][j + 1] > 0) { //ひっくり返り得る数を表示
        //font(canPut[i + 1][j + 1]).draw(Origin.x + gridSize * i +5, Origin.y + gridSize * j + 5, Palette::White);
        smallStone[i][j].draw(Palette::Darkgoldenrod);
      }
      //////////終わり
    }
  }
  
  

	//クリックしたところに石を置く
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (cell[i][j].leftClicked() && innerCell[i + 1][j + 1] == 0 && canPut[i + 1][j + 1] > 0) {
				turn++;
				innerCell[i + 1][j + 1] = turn % 2 + 1; //1なら黒、2なら白
				record[turn].x = i + 1; //置いた場所を記録
				record[turn].y = j + 1;
				record[turn].turn = turn;
				record[turn].status = innerCell[i + 1][j + 1]; //色を記録
				put = true; //探索させる
			}

			if (innerCell[i + 1][j + 1] == 1) {
				//stone[i][j].draw(Palette::Black);
			}
			else if (innerCell[i + 1][j + 1] == 2) {
				//stone[i][j].draw(Palette::White);
			}
      //石を表示
			if (deepCell[turn][i + 1][j + 1] == 1) {
				stone[i][j].draw(Palette::Black);
			}
			else if (deepCell[turn][i + 1][j + 1] == 2) {
				stone[i][j].draw(Palette::White);
			}
		}
	}


	//ランダムに置く
	int maxPut = 0;
	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			maxPut = Max(canPut[i][j], maxPut);
		}
	}

  //スコアが最大の場所を探す
  int maxScore = -1000;
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      if (canPut[i][j] > 0) {
        maxScore = Max(maxScore, score[i][j]);
      }
    }
  }
  //置く場所を決定する
  
	//Print << U"max:" << maxPut;
	int32 I, J;
  SimpleGUI::CheckBox(ai, U"Computer", Vec2(10, 150));
	if (ai == true && turn % 2 == 0) {
		int32 m = Random() * 100, n = Random() * 100;
		for (int i = 1; i < 9; i++) {
			for (int j = 1; j < 9; j++) {
				if (m >= 50 && n >= 50) {
					I = i;
          J = j;
				}
				else if(m >= 50 && n < 50){
					I = j;
          J = i;
				} 
        else if (m < 50 && n >= 50) {
          I = j;
					J = 9 - i;
				}
				else if(true){
          I = 9 - j;
					J = 9 - i;
        }
        else if (false) {

        }
      
				if (innerCell[I][J] == 0 && canPut[I][J] > 0 && (score[I][J] == maxScore || canPut[I][J] == maxPut && turn >= 55)) {
					turn++;
					innerCell[I][J] = turn % 2 + 1; //1なら黒、2なら白
					record[turn].x = I; //置いた場所を記録
					record[turn].y = J;
					record[turn].turn = turn;
					record[turn].status = innerCell[I][J]; //色を記録
					put = true; //探索させる
				}
				if (put == true) {
					break;
				}
			}
			if (put == true) {
				break;
			}
		}
		//スキップしてもらう
		if (put == false) {
			//turn++;
      pleaseSkip = true;
			//font(U"スキップしてください").draw(Origin.x + gridSize * 2, Origin.y + gridSize * 3);
		}
	}
  
  
	//そのターンで置かれた場所に印をつける
	if (record[turn].x > 0 && record[turn].y > 0) {
		smallStone[record[turn].x - 1][record[turn].y - 1].draw(Palette::Aqua);
	}

	//変更をdeepCellにも反映させる
	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			deepCell[turn][i][j] = innerCell[i][j];
		}
	}

	int ans = 0;
	//石を自動でひっくり返す
	//SimpleGUI::CheckBox(search, U"石を返す", Vec2(500, 40));
	if (search == true && put == true) { //チェックボックスtrueで石が置かれたら探索
		ans = 0;
		for (int k = 0; k < 8; k++) {
			ans += flip_stone(k, record[turn], -1); //ひっくり返す
		}
	}
	//Print << U"返した石の数は"<<ans;

	//手数、順番を表示
	font(U"手数", turn).draw(10, 200);
	if (turn % 2 == 0) {
		font(U"白の番").draw(10, 250);
	}
	else {
		font(U"黒の番").draw(10, 250);
	}

	//石の数を表示
	int32 black = 0, white = 0;
	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			if (innerCell[i][j] == 1) {
				black++;
			}
			else if (innerCell[i][j] == 2) {
				white++;
			}
		}
	}
	font(black).draw(700, 100, Palette::Black);
	font(white).draw(700, 150);

    //RectF(Arg::center(Origin.x + gridSize * 4, Origin.y + gridSize * (-0.8)), gridSize * 6, gridSize).draw(ColorF(0.4, 0.7, 0.4, 0.5));
  if (black + white >= 64) {
    pleaseSkip = false;

    if (black > white) {
      font(U"黒の勝ち！").drawAt(Origin.x + gridSize * 4, Origin.y + gridSize * (-0.8));
    }
    else if (black < white) {
      font(U"白の勝ち！").drawAt(Origin.x + gridSize * 4, Origin.y + gridSize * (-0.8));
    }
    else {
      font(U"引き分け！").drawAt(Origin.x + gridSize * 4, Origin.y + gridSize * (-0.8));
    }
  }
  if (black == 0) {
    font(U"白の完勝！").drawAt(Origin.x + gridSize * 4, Origin.y + gridSize * (-0.8));
  }
  else if (white == 0) {
    font(U"黒の完勝！").drawAt(Origin.x + gridSize * 4, Origin.y + gridSize * (-0.8));
  }
  if (pleaseSkip == true) {
    font(U"スキップしてください").drawAt(Origin.x + gridSize * 4, Origin.y + gridSize * (-0.8));
  }
  /*
	if (oncePerSecond % 60 == 0) {
		Print << staticScore(0);
	}
  */
	put = false; //元に戻す


	//Print << record[turn].x <<record[turn].y<<record[turn].status;

	//石を消去するボタン
	if (SimpleGUI::Button(U"Clear", Vec2(10, 50))) {
		for (int i = 1; i < 9; i++) {
			for (int j = 1; j < 9; j++) {
				innerCell[i][j] = 0;
				record[i * 10 + j] = { 0 }; //記録をリセット
				for (int t = 0; t < DEPTH; t++) {
					deepCell[t][i][j] = 0;
				}
			}
		}
		turn = 1; //手数もリセット
		innerCell[4][4] = deepCell[1][4][4] = 1;
		innerCell[4][5] = deepCell[1][4][5] = 2;
		innerCell[5][4] = deepCell[1][5][4] = 2;
		innerCell[5][5] = deepCell[1][5][5] = 1;
    pleaseSkip = false;
	}

	//スキップするボタン
	if (SimpleGUI::Button(U"Skip", Vec2(10, 100))) {
		turn++;
    pleaseSkip = false;
	}

  //タイトル画面に戻る
	if (SimpleGUI::Button(U"end", Vec2(10,500))) {
    Scene::SetBackground(ColorF(0.2, 0.8, 0.4));

    //盤面のリセット
    for (int i = 1; i < 9; i++) {
      for (int j = 1; j < 9; j++) {
        innerCell[i][j] = 0;
        record[i * 10 + j] = { 0 }; //記録をリセット
        for (int t = 0; t < DEPTH; t++) {
          deepCell[t][i][j] = 0;
        }
      }
    }
    turn = 1; //手数もリセット
    innerCell[4][4] = deepCell[1][4][4] = 1;
    innerCell[4][5] = deepCell[1][4][5] = 2;
    innerCell[5][4] = deepCell[1][5][4] = 2;
    innerCell[5][5] = deepCell[1][5][5] = 1;
    pleaseSkip = false;

    changeScene(State::Title);
    getData().highScore = 999;
      //Max(getData().highScore, m_score);
	}
  
}

void Game::draw() const
{
	/****************************************************************************/
	/*
	FontAsset(U"Score")(m_score).drawAt(Scene::Center().x, 30);

	// すべてのブロックを描画する
	for (const auto& block : m_blocks)
	{
		block.stretched(-1).draw(HSV(block.y - 40));
	}

	// ボールを描く
	m_ball.draw();

	// パドルを描く
	m_paddle.draw();
	*/
	/****************************************************************************/

}

int flip_stone(int k, Coordinate c, int a) {
	int flag = 0;
	int X = 0, Y = 0, Z = 0;
  if (a >= 1) {
		Z = a;
	}

	//switch文で探索方向を決める
	switch (k) {
	case 0:
		X = 1, Y = 0;
		break;
	case 1:
		X = 1; Y = 1;
		break;
	case 2:
		X = 0, Y = 1;
		break;
	case 3:
		X = -1; Y = 1;
		break;
	case 4:
		X = -1, Y = 0;
		break;
	case 5:
		X = -1, Y = -1;
		break;
	case 6:
		X = 0, Y = -1;
		break;
	case 7:
		X = 1, Y = -1;
		break;
	default:
		break;
	}

	//探索部
	int i = c.x + X, j = c.y + Y;
	int n = 0;
	int m = 0;
	int ans = 0;
	int tmp = 0;

	sp = 0;
	while (1)
	{
		//Print << c.turn;
		if (deepCell[c.turn][i][j] > 0) {
			for (int count = 0; deepCell[c.turn][i][j] != c.status && deepCell[c.turn][i][j] > 0; count++) {
				push(i * 10 + j);
				n++;
        //if(Z!=0)
				  //Print << U"pushされた"<<i*10 + j;
				i += X;
				j += Y;
				m = 1;
			}
			//break;
			if (deepCell[c.turn][i][j] == c.status && m == 1) {
				while (n > 0) {
					tmp = pop();
					if (a == -1) {
						innerCell[tmp / 10][tmp % 10] = c.status;
						deepCell[c.turn][tmp / 10][tmp % 10] = c.status;
					}
					else if (a >= 1) {
						deepCell[c.turn][tmp / 10][tmp % 10] = c.status;
            //Print << U"deep" << c.turn;
					}
					//Print << U"popされた"<<tmp;
					ans++;
					n--;
				}
			}
		}
		break;
	}
	return ans;
}

void push(int i) {
	stack[sp++] = i;
}

int pop() {
	int tmp = stack[--sp];
	stack[sp + 1] = 0;
	return tmp;
}

//d手先でのスコアを計算する
int staticScore(int d) {
  d += turn;
	//s = 0;
	//int k = 0;
	int ans = 0;
	int s_canPut[10][10] = { 0 };
 
	

  //今の番のプレイヤーが置ける場所の数をカウント
	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			for (int k = 0; k < 8; k++) {
				Coordinate c = { i, j, (d + 1) % 2 + 1, d };
				s_canPut[i][j] += flip_stone(k, c, 0); //ひっくり返さない
				if (innerCell[i][j] != 0) {
					//s_canPut[i][j] = 0; //置ける場所でなかったら0に戻す
				}
			}
      //Print << U"s_"<<s_canPut[i][j];
		}
	}
	for (int i = 1; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (s_canPut[i][j] > 0) {
				ans++;
			}
		}
	}
  //Print << d << U":" <<U"+"<<ans;
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      s_canPut[i][j] = 0;
    }
  }
  
  //相手プレイヤ－が置ける場所の数をカウント
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      for (int k = 0; k < 8; k++) {
        Coordinate c = { i, j, (d + 2) % 2 + 1, d };
        if (d > turn) {
          s_canPut[i][j] += flip_stone(k, c, 0); //ひっくり返さない
        }
        else {
          s_canPut[i][j] += flip_stone(k, c, 0); //ひっくり返さない
        }
        if (innerCell[i][j] != 0) {
          s_canPut[i][j] = 0; //置ける場所でなかったら0に戻す
        }
      }
    }
  }
  int mm = 0;
  for (int i = 1; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (s_canPut[i][j] > 0) {
        ans--;
        mm++;
      }
    }
  }
  //Print << d << U":" <<U"-"<<mm;
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      s_canPut[i][j] = 0;
    }
  }
  
  ans *= 15;

  int tu = (turn - 1) % 2 + 1;
  
  //角
  int32 corner = 10;
  if (deepCell[d][1][1] == tu) {
    ans += 15;
    if (deepCell[d][1][2] == tu || deepCell[d][2][1] == tu || deepCell[d][2][2] == tu) {
      ans += corner;
    }
    //Print << U"角";
  }
  if (deepCell[d][1][8] == tu) {
    ans += 15;
    if (deepCell[d][1][7] == tu || deepCell[d][2][8] == tu || deepCell[d][2][7] == tu) {
      ans += corner;
    }
    //Print << U"角";
  }
  if (deepCell[d][8][1] == tu) {
    ans += 15;
    if (deepCell[d][8][2] == tu || deepCell[d][7][1] == tu || deepCell[d][7][2] == tu) {
      ans += corner;
    }
    //Print << U"角";
  }
  if (deepCell[d][8][8] == tu) {
    ans += 15;
    if (deepCell[d][8][7] == tu || deepCell[d][7][8] == tu || deepCell[d][7][7] == tu) {
      ans += corner;
    }
    //Print << U"角";
  }

  //1,1の周辺
  //横
  if (deepCell[d][3][1] == tu) {
    if (deepCell[d][2][1] == tu && deepCell[d][1][1] == 0) {
      ans -= 10;
    }
    else if (deepCell[d][2][1] == 0 && deepCell[d][1][1] == 0) {
      ans += 3;
    }
  }
  //斜め
  if (deepCell[d][3][3] == tu) {
    if (deepCell[d][2][2] == tu && deepCell[d][1][1] == 0) {
      ans -= 10;
    }
    else if (deepCell[d][2][2] == 0 && deepCell[d][1][1] == 0) {
      ans += 3;
    }
  }
  //縦
  if (deepCell[d][1][3] == tu) {
    if (deepCell[d][1][2] == tu && deepCell[d][1][1] == 0) {
      ans -= 10;
    }
    else if (deepCell[d][1][2] == 0 && deepCell[d][1][1] == 0) {
      ans += 3;
    }
  }
  //8,1の周辺
  //横
  if (deepCell[d][6][1] == tu) {
    if (deepCell[d][7][1] == tu && deepCell[d][8][1] == 0) {
      ans -= 10;
    }
    else if (deepCell[d][7][1] == 0 && deepCell[d][8][1] == 0) {
      ans += 3;
    }
  }
  //斜め
  if (deepCell[d][6][3] == tu) {
    if (deepCell[d][7][2] == tu && deepCell[d][8][1] == 0) {
      ans -= 10;
    }
    else if (deepCell[d][7][2] == 0 && deepCell[d][8][1] == 0) {
      ans += 3;
    }
  }
  //縦
  if (deepCell[d][8][3] == tu) {
    if (deepCell[d][8][2] == tu && deepCell[d][8][1] == 0) {
      ans -= 10;
    }
    else if (deepCell[d][8][2] == 0 && deepCell[d][8][1] == 0) {
      ans += 3;
    }
  }
  //1,8の周辺
  //横
  if (deepCell[d][3][8] == tu) {
    if (deepCell[d][2][8] == tu && deepCell[d][1][8] == 0) {
      ans -= 10;
    }
    else if (deepCell[d][2][8] == 0 && deepCell[d][1][8] == 0) {
      ans += 3;
    }
  }
  //斜め
  if (deepCell[d][3][6] == tu) {
    if (deepCell[d][2][7] == tu && deepCell[d][1][8] == 0) {
      ans -= 10;
    }
    else if (deepCell[d][2][7] == 0 && deepCell[d][1][8] == 0) {
      ans += 3;
    }
  }
  //縦
  if (deepCell[d][1][6] == tu) {
    if (deepCell[d][1][7] == tu && deepCell[d][1][8] == 0) {
      ans -= 10;
    }
    else if (deepCell[d][1][7] == 0 && deepCell[d][1][8] == 0) {
      ans += 3;
    }
  }
  //8,8の周辺
  //横
  if (deepCell[d][6][8] == tu) {
    if (deepCell[d][7][8] == tu && deepCell[d][8][8] == 0) {
      ans -= 10;
    }
    else if (deepCell[d][7][8] == 0 && deepCell[d][8][8] == 0) {
      ans += 3;
    }
  }
  //斜め
  if (deepCell[d][6][6] == tu) {
    if (deepCell[d][7][7] == tu && deepCell[d][8][8] == 0) {
      ans -= 10;
    }
    else if (deepCell[d][7][7] == 0 && deepCell[d][8][8] == 0) {
      ans += 3;
    }
  }
  //縦
  if (deepCell[d][8][6] == tu) {
    if (deepCell[d][8][7] == tu && deepCell[d][8][8] == 0) {
      ans -= 10;
    }
    else if (deepCell[d][8][7] == 0 && deepCell[d][8][8] == 0) {
      ans += 3;
    }
  }
  
	return ans;
}

//xは1-8,yも1-8, zは深さ
//cellごとのスコアを付ける
//置けないところなら0
int cellScore(int x, int y, int z) { 
  //int ans = deepCell[z + turn][x][y];
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      for (int k = 1; k < z + 1; k++) {
        deepCell[turn + z][i][j] = deepCell[turn][i][j];
      }
    }
  }

  Coordinate c ;
  int32 score = 0;
  //着手してみてスコアを付ける
  if (deepCell[z + turn][x][y] == 0) {
    
    deepCell[z + turn][x][y] = (turn + 1) % 2 + 1; //1なら黒、2なら白
    c.x = x; //置いた場所を記録
    c.y = y;
    c.turn = turn + z;
    c.status = deepCell[z + turn][x][y]; //色を記録
    //put = true; //探索させる

    for (int k = 0; k < 8; k++) {
      int ans = flip_stone(k, c, z); //ひっくり返す
    }
    score = staticScore(z);
  }
  
  //スコア計算したらもとに戻す
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      for (int k = 1; k < z + 1; k++) {
        deepCell[k + turn][i][j] = deepCell[turn][i][j];
      }
    }
  }
  
  return score;
}
